/*
 * Node Skipping Attack Variation - Syringe Pump Simulator
 * 
 * This variation models a control-flow attack that skips critical nodes
 * in the execution flow, bypassing safety checks and volume calculations.
 * 
 * Attack scenarios:
 * 1. Skip volume limit validation
 * 2. Skip step calculation (leading to incorrect dosing)
 * 3. Skip state updates (inconsistent tracking)
 * 4. Skip timing delays (unsafe motor operation)
 * 
 * Based on the original syringe pump implementation from CFLAT experiments
 */

#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <string.h>

/* -- Constants (from Arduino code) -- */
#define SYRINGE_VOLUME_ML 30.0
#define SYRINGE_BARREL_LENGTH_MM 80.0
#define THREADED_ROD_PITCH 1.25
#define STEPS_PER_REVOLUTION 200.0
#define MICROSTEPS_PER_STEP 16.0
#define SPEED_MICROSECONDS_DELAY 100

/* Attack simulation flags */
#define ATTACK_SKIP_VOLUME_CHECK 1
#define ATTACK_SKIP_STEP_CALC 2
#define ATTACK_SKIP_STATE_UPDATE 3
#define ATTACK_SKIP_TIMING_DELAY 4

/* -- Enums -- */
typedef enum {
    PUSH,
    PULL
} Direction;

/* -- Global state -- */
typedef struct {
    long ustepsPerMM;
    long ustepsPerML;
    float mLBolus;
    float mLUsed;
    long stepperPos;
    int attack_mode;  // Attack simulation mode
} PumpState;

/* -- Function prototypes -- */
void init_pump(PumpState *pump);
void bolus_attacked(PumpState *pump, Direction direction);
void simulate_motor_step_attacked(long steps, int speed_us, int attack_mode);
void print_status(PumpState *pump);
void run_attack_demonstrations(PumpState *pump);
double get_time_us(void);
double measure_execution_time(void (*func)(PumpState*), PumpState *pump);

/* -- Main -- */
int main(void) {
    PumpState pump;
    
    printf("=== Node Skipping Attack Demonstration ===\n");
    printf("Simulating control-flow attacks on syringe pump\n");
    printf("WARNING: This is for research purposes only!\n\n");
    
    init_pump(&pump);
    print_status(&pump);
    
    // Demonstrate various node-skipping attacks
    run_attack_demonstrations(&pump);
    
    printf("\n=== Attack Demonstration Complete ===\n");
    print_status(&pump);
    
    return 0;
}

/* Initialize pump state */
void init_pump(PumpState *pump) {
    pump->ustepsPerMM = (long)(MICROSTEPS_PER_STEP * STEPS_PER_REVOLUTION / THREADED_ROD_PITCH);
    pump->ustepsPerML = (long)((MICROSTEPS_PER_STEP * STEPS_PER_REVOLUTION * SYRINGE_BARREL_LENGTH_MM) / 
                                (SYRINGE_VOLUME_ML * THREADED_ROD_PITCH));
    pump->mLBolus = 0.5;  // default 0.5 mL bolus
    pump->mLUsed = 0.0;
    pump->stepperPos = 0;
    pump->attack_mode = 0;  // No attack initially
    
    printf("Pump initialized:\n");
    printf("  Steps per mm: %ld\n", pump->ustepsPerMM);
    printf("  Steps per mL: %ld\n", pump->ustepsPerML);
    printf("  Default bolus: %.3f mL\n\n", pump->mLBolus);
}

/* Simulate a bolus with node-skipping attacks */
void bolus_attacked(PumpState *pump, Direction direction) {
    long steps;
    
    // ATTACK 1: Skip volume limit validation
    if (pump->attack_mode == ATTACK_SKIP_VOLUME_CHECK) {
        printf("[ATTACK] Skipping volume limit check - allowing dangerous overdose!\n");
        // Normal code would check: if (pump->mLBolus > MAX_SAFE_BOLUS) return;
        // Attack skips this critical safety check
    } else {
        // Normal safety check (simplified)
        if (pump->mLBolus > 5.0) {
            printf("WARNING: Bolus too large, limiting to 5.0 mL\n");
            pump->mLBolus = 5.0;
        }
    }
    
    // ATTACK 2: Skip step calculation
    if (pump->attack_mode == ATTACK_SKIP_STEP_CALC) {
        printf("[ATTACK] Skipping step calculation - using corrupted value!\n");
        steps = 999999;  // Malicious step count
    } else {
        steps = (long)(pump->mLBolus * pump->ustepsPerML);
    }
    
    if (direction == PUSH) {
        printf("PUSH: Dispensing %.3f mL (%ld steps)...", pump->mLBolus, steps);
        fflush(stdout);
        
        simulate_motor_step_attacked(steps, SPEED_MICROSECONDS_DELAY, pump->attack_mode);
        
        // ATTACK 3: Skip state updates
        if (pump->attack_mode != ATTACK_SKIP_STATE_UPDATE) {
            pump->mLUsed += pump->mLBolus;
            pump->stepperPos += steps;
        } else {
            printf("[ATTACK] Skipping state update - system state now inconsistent!\n");
        }
    } 
    else if (direction == PULL) {
        printf("PULL: Retracting %.3f mL (%ld steps)...", pump->mLBolus, steps);
        fflush(stdout);
        
        simulate_motor_step_attacked(steps, SPEED_MICROSECONDS_DELAY, pump->attack_mode);
        
        // ATTACK 3: Skip state updates
        if (pump->attack_mode != ATTACK_SKIP_STATE_UPDATE) {
            if ((pump->mLUsed - pump->mLBolus) > 0) {
                pump->mLUsed -= pump->mLBolus;
            } else {
                pump->mLUsed = 0.0;
            }
            pump->stepperPos -= steps;
        } else {
            printf("[ATTACK] Skipping state update - system state now inconsistent!\n");
        }
    }
    
    printf(" Done!\n");
}

/* Simulate motor stepping with timing attacks */
void simulate_motor_step_attacked(long steps, int speed_us, int attack_mode) {
    struct timespec delay;
    delay.tv_sec = 0;
    delay.tv_nsec = speed_us * 1000;
    
    // ATTACK 4: Skip timing delays (dangerous for motor)
    if (attack_mode == ATTACK_SKIP_TIMING_DELAY) {
        printf("[ATTACK] Skipping timing delays - motor may overheat/malfunction!\n");
        // Skip all nanosleep calls - this would damage real hardware
        for (long i = 0; i < steps; i++) {
            // No delays - just computational load
            if (i % 100 == 0) {
                volatile double result = sin((double)i / 100.0) * cos((double)i / 50.0);
                result = result * result;
            }
        }
        return;
    }
    
    // Normal timing-critical motor control
    for (long i = 0; i < steps; i++) {
        nanosleep(&delay, NULL);  // HIGH pulse
        nanosleep(&delay, NULL);  // LOW pulse
        
        if (i % 100 == 0) {
            volatile double result = sin((double)i / 100.0) * cos((double)i / 50.0);
            result = result * result;
        }
    }
}

/* Print current pump status */
void print_status(PumpState *pump) {
    printf("Status: Used %.3f mL, Position %ld steps, Bolus size %.3f mL, Attack mode: %d\n", 
           pump->mLUsed, pump->stepperPos, pump->mLBolus, pump->attack_mode);
}

/* Demonstrate various node-skipping attacks */
void run_attack_demonstrations(PumpState *pump) {
    double exec_time;
    
    printf("=== Node Skipping Attack Demonstrations ===\n\n");
    
    // Normal operation baseline
    printf("--- Baseline (Normal Operation) ---\n");
    pump->attack_mode = 0;
    pump->mLBolus = 0.1;
    exec_time = measure_execution_time((void(*)(PumpState*))bolus_attacked, pump);
    printf("Execution time: %.2f ms\n", exec_time / 1000.0);
    print_status(pump);
    printf("\n");
    
    // Attack 1: Skip volume validation
    printf("--- Attack 1: Skip Volume Validation ---\n");
    pump->attack_mode = ATTACK_SKIP_VOLUME_CHECK;
    pump->mLBolus = 10.0;  // Dangerously large dose
    exec_time = measure_execution_time((void(*)(PumpState*))bolus_attacked, pump);
    printf("Execution time: %.2f ms\n", exec_time / 1000.0);
    print_status(pump);
    printf("\n");
    
    // Attack 2: Skip step calculation
    printf("--- Attack 2: Skip Step Calculation ---\n");
    pump->attack_mode = ATTACK_SKIP_STEP_CALC;
    pump->mLBolus = 0.1;
    exec_time = measure_execution_time((void(*)(PumpState*))bolus_attacked, pump);
    printf("Execution time: %.2f ms\n", exec_time / 1000.0);
    print_status(pump);
    printf("\n");
    
    // Attack 3: Skip state updates
    printf("--- Attack 3: Skip State Updates ---\n");
    pump->attack_mode = ATTACK_SKIP_STATE_UPDATE;
    pump->mLBolus = 0.5;
    exec_time = measure_execution_time((void(*)(PumpState*))bolus_attacked, pump);
    printf("Execution time: %.2f ms\n", exec_time / 1000.0);
    print_status(pump);
    printf("\n");
    
    // Attack 4: Skip timing delays
    printf("--- Attack 4: Skip Timing Delays ---\n");
    pump->attack_mode = ATTACK_SKIP_TIMING_DELAY;
    pump->mLBolus = 0.01;  // Small dose to demonstrate timing difference
    exec_time = measure_execution_time((void(*)(PumpState*))bolus_attacked, pump);
    printf("Execution time: %.2f ms\n", exec_time / 1000.0);
    print_status(pump);
    printf("\n");
}

/* Measure execution time of a function */
double measure_execution_time(void (*func)(PumpState*), PumpState *pump) {
    double start_time = get_time_us();
    func(pump);
    double end_time = get_time_us();
    return end_time - start_time;
}

/* Get current time in microseconds */
double get_time_us(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec * 1000000.0 + (double)ts.tv_nsec / 1000.0;
}