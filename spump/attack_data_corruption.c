/*
 * Data Corruption Attack Variation - Syringe Pump Simulator
 * 
 * This variation models attacks that corrupt critical data structures
 * affecting control flow, particularly focusing on loop counters,
 * dosage amounts, timing parameters, and state variables.
 * 
 * Attack scenarios:
 * 1. Loop counter corruption (affects motor stepping duration)
 * 2. Dosage amount corruption (affects volume dispensed)
 * 3. Timing parameter corruption (affects motor speed/safety)
 * 4. State corruption (position tracking, volume tracking)
 * 5. Configuration parameter corruption (steps per mL calculations)
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

/* Data corruption attack types */
typedef enum {
    ATTACK_NONE = 0,
    ATTACK_LOOP_COUNTER,        // Corrupt motor step loop counters
    ATTACK_DOSAGE_AMOUNT,       // Corrupt bolus volume amount
    ATTACK_TIMING_PARAMS,       // Corrupt motor timing delays
    ATTACK_STATE_CORRUPTION,    // Corrupt position/volume tracking
    ATTACK_CONFIG_PARAMS        // Corrupt calibration parameters
} DataAttackMode;

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
    DataAttackMode attack_mode;
    
    // Attack simulation variables
    int corruption_factor;      // Multiplier for corruption severity
    long corrupted_steps;       // Store original vs corrupted step counts
    int timing_corruption;      // Timing delay corruption
    float dosage_corruption;    // Dosage amount corruption
} PumpState;

/* -- Function prototypes -- */
void init_pump(PumpState *pump);
void bolus_with_data_corruption(PumpState *pump, Direction direction);
void simulate_motor_step_corrupted(long *steps, int *speed_us, DataAttackMode attack_mode);
void corrupt_data_structures(PumpState *pump);
void print_status(PumpState *pump);
void run_data_corruption_attacks(PumpState *pump);
double get_time_us(void);
double measure_execution_time(void (*func)(PumpState*), PumpState *pump);

/* -- Main -- */
int main(void) {
    PumpState pump;
    
    printf("=== Data Corruption Attack Demonstration ===\n");
    printf("Simulating data corruption attacks affecting control flow\n");
    printf("WARNING: This is for research purposes only!\n\n");
    
    init_pump(&pump);
    print_status(&pump);
    
    // Demonstrate various data corruption attacks
    run_data_corruption_attacks(&pump);
    
    printf("\n=== Attack Demonstration Complete ===\n");
    print_status(&pump);
    
    return 0;
}

/* Initialize pump state */
void init_pump(PumpState *pump) {
    pump->ustepsPerMM = (long)(MICROSTEPS_PER_STEP * STEPS_PER_REVOLUTION / THREADED_ROD_PITCH);
    pump->ustepsPerML = (long)((MICROSTEPS_PER_STEP * STEPS_PER_REVOLUTION * SYRINGE_BARREL_LENGTH_MM) / 
                                (SYRINGE_VOLUME_ML * THREADED_ROD_PITCH));
    pump->mLBolus = 0.5;
    pump->mLUsed = 0.0;
    pump->stepperPos = 0;
    pump->attack_mode = ATTACK_NONE;
    pump->corruption_factor = 1;
    pump->corrupted_steps = 0;
    pump->timing_corruption = 0;
    pump->dosage_corruption = 0.0;
    
    printf("Pump initialized:\n");
    printf("  Steps per mm: %ld\n", pump->ustepsPerMM);
    printf("  Steps per mL: %ld\n", pump->ustepsPerML);
    printf("  Default bolus: %.3f mL\n\n", pump->mLBolus);
}

/* Simulate a bolus with various data corruptions */
void bolus_with_data_corruption(PumpState *pump, Direction direction) {
    long steps;
    int speed_us = SPEED_MICROSECONDS_DELAY;
    float original_bolus = pump->mLBolus;
    
    // Apply data corruptions before calculations
    corrupt_data_structures(pump);
    
    // Calculate steps (potentially with corrupted parameters)
    steps = (long)(pump->mLBolus * pump->ustepsPerML);
    
    if (direction == PUSH) {
        printf("PUSH: Dispensing %.3f mL (%ld steps)...", pump->mLBolus, steps);
        if (pump->attack_mode != ATTACK_NONE) {
            printf("\n[ATTACK] Data corruption detected during operation!");
        }
        fflush(stdout);
        
        // Motor stepping with potential corruption
        simulate_motor_step_corrupted(&steps, &speed_us, pump->attack_mode);
        
        // Update state (potentially corrupted)
        if (pump->attack_mode == ATTACK_STATE_CORRUPTION) {
            printf("[ATTACK] Corrupting state updates!\n");
            pump->mLUsed += (pump->mLBolus * 3.5);  // Incorrect state update
            pump->stepperPos += (steps / 2);         // Incorrect position update
        } else {
            pump->mLUsed += pump->mLBolus;
            pump->stepperPos += steps;
        }
    } 
    else if (direction == PULL) {
        printf("PULL: Retracting %.3f mL (%ld steps)...", pump->mLBolus, steps);
        if (pump->attack_mode != ATTACK_NONE) {
            printf("\n[ATTACK] Data corruption detected during operation!");
        }
        fflush(stdout);
        
        simulate_motor_step_corrupted(&steps, &speed_us, pump->attack_mode);
        
        if (pump->attack_mode == ATTACK_STATE_CORRUPTION) {
            printf("[ATTACK] Corrupting state updates!\n");
            pump->mLUsed = 0.0;  // Incorrect - should subtract properly
            pump->stepperPos -= (steps * 2);  // Incorrect position update
        } else {
            if ((pump->mLUsed - pump->mLBolus) > 0) {
                pump->mLUsed -= pump->mLBolus;
            } else {
                pump->mLUsed = 0.0;
            }
            pump->stepperPos -= steps;
        }
    }
    
    // Restore original bolus amount if it was corrupted
    if (pump->attack_mode == ATTACK_DOSAGE_AMOUNT) {
        pump->mLBolus = original_bolus;
    }
    
    printf(" Done!\n");
}

/* Apply various data corruptions */
void corrupt_data_structures(PumpState *pump) {
    switch (pump->attack_mode) {
        case ATTACK_DOSAGE_AMOUNT:
            printf("[ATTACK] Corrupting dosage amount: %.3f -> ", pump->mLBolus);
            pump->mLBolus *= pump->corruption_factor;  // Multiply dosage
            pump->dosage_corruption = pump->mLBolus;
            printf("%.3f mL\n", pump->mLBolus);
            break;
            
        case ATTACK_CONFIG_PARAMS:
            printf("[ATTACK] Corrupting configuration parameters!\n");
            printf("Original ustepsPerML: %ld -> ", pump->ustepsPerML);
            pump->ustepsPerML /= pump->corruption_factor;  // Corrupt calibration
            printf("%ld\n", pump->ustepsPerML);
            break;
            
        case ATTACK_TIMING_PARAMS:
            printf("[ATTACK] Corrupting timing parameters!\n");
            pump->timing_corruption = SPEED_MICROSECONDS_DELAY / pump->corruption_factor;
            printf("Original delay: %d -> %d microseconds\n", 
                   SPEED_MICROSECONDS_DELAY, pump->timing_corruption);
            break;
            
        default:
            break;
    }
}

/* Simulate motor stepping with data corruption attacks */
void simulate_motor_step_corrupted(long *steps, int *speed_us, DataAttackMode attack_mode) {
    struct timespec delay;
    long original_steps = *steps;
    int original_speed = *speed_us;
    
    // Apply attack-specific corruptions
    switch (attack_mode) {
        case ATTACK_LOOP_COUNTER:
            printf("\n[ATTACK] Corrupting loop counter: %ld -> ", *steps);
            *steps *= 5;  // Corrupt step count - causes excessive motor movement
            printf("%ld steps\n", *steps);
            break;
            
        case ATTACK_TIMING_PARAMS:
            *speed_us = *speed_us / 10;  // Make timing 10x faster - dangerous for motor
            break;
            
        default:
            break;
    }
    
    delay.tv_sec = 0;
    delay.tv_nsec = (*speed_us) * 1000;
    
    // Execute motor loop (potentially with corrupted parameters)
    for (long i = 0; i < *steps; i++) {
        // Simulate motor pulses
        nanosleep(&delay, NULL);  // HIGH pulse
        nanosleep(&delay, NULL);  // LOW pulse
        
        // Add computational load
        if (i % 100 == 0) {
            volatile double result = sin((double)i / 100.0) * cos((double)i / 50.0);
            result = result * result;
            
            // Additional corruption mid-loop
            if (attack_mode == ATTACK_LOOP_COUNTER && i == *steps / 2) {
                printf("\n[ATTACK] Mid-loop corruption! Changing step count during execution!\n");
                *steps *= 2;  // Further corrupt loop counter mid-execution
            }
        }
    }
    
    // Log corruption effects
    if (attack_mode == ATTACK_LOOP_COUNTER) {
        printf("[ATTACK] Loop executed %ld steps instead of %ld (%.1fx longer)\n", 
               *steps, original_steps, (float)*steps / original_steps);
    }
    if (attack_mode == ATTACK_TIMING_PARAMS) {
        printf("[ATTACK] Timing corrupted: %d -> %d microseconds (%.1fx faster)\n", 
               original_speed, *speed_us, (float)original_speed / *speed_us);
    }
}

/* Print current pump status */
void print_status(PumpState *pump) {
    printf("Status: Used %.3f mL, Position %ld steps, Bolus %.3f mL, Attack: %d\n", 
           pump->mLUsed, pump->stepperPos, pump->mLBolus, pump->attack_mode);
    
    if (pump->attack_mode != ATTACK_NONE) {
        printf("       Corruption factor: %d, Dosage corruption: %.3f, Timing corruption: %d\n",
               pump->corruption_factor, pump->dosage_corruption, pump->timing_corruption);
    }
}

/* Demonstrate various data corruption attacks */
void run_data_corruption_attacks(PumpState *pump) {
    double exec_time;
    
    printf("=== Data Corruption Attack Demonstrations ===\n\n");
    
    // Normal operation baseline
    printf("--- Baseline (Normal Operation) ---\n");
    pump->attack_mode = ATTACK_NONE;
    pump->mLBolus = 0.1;
    exec_time = measure_execution_time((void(*)(PumpState*))bolus_with_data_corruption, pump);
    printf("Execution time: %.2f ms\n", exec_time / 1000.0);
    print_status(pump);
    printf("\n");
    
    // Attack 1: Loop counter corruption
    printf("--- Attack 1: Loop Counter Corruption ---\n");
    pump->attack_mode = ATTACK_LOOP_COUNTER;
    pump->corruption_factor = 3;
    pump->mLBolus = 0.05;  // Small dose to see corruption effect
    exec_time = measure_execution_time((void(*)(PumpState*))bolus_with_data_corruption, pump);
    printf("Execution time: %.2f ms\n", exec_time / 1000.0);
    print_status(pump);
    printf("\n");
    
    // Attack 2: Dosage amount corruption
    printf("--- Attack 2: Dosage Amount Corruption ---\n");
    pump->attack_mode = ATTACK_DOSAGE_AMOUNT;
    pump->corruption_factor = 10;  // 10x overdose
    pump->mLBolus = 0.1;
    exec_time = measure_execution_time((void(*)(PumpState*))bolus_with_data_corruption, pump);
    printf("Execution time: %.2f ms\n", exec_time / 1000.0);
    print_status(pump);
    printf("\n");
    
    // Attack 3: Timing parameter corruption
    printf("--- Attack 3: Timing Parameter Corruption ---\n");
    pump->attack_mode = ATTACK_TIMING_PARAMS;
    pump->corruption_factor = 5;  // 5x faster timing
    pump->mLBolus = 0.01;  // Small dose to see timing effect
    exec_time = measure_execution_time((void(*)(PumpState*))bolus_with_data_corruption, pump);
    printf("Execution time: %.2f ms\n", exec_time / 1000.0);
    print_status(pump);
    printf("\n");
    
    // Attack 4: State corruption
    printf("--- Attack 4: State Corruption ---\n");
    pump->attack_mode = ATTACK_STATE_CORRUPTION;
    pump->mLBolus = 0.5;
    exec_time = measure_execution_time((void(*)(PumpState*))bolus_with_data_corruption, pump);
    printf("Execution time: %.2f ms\n", exec_time / 1000.0);
    print_status(pump);
    printf("\n");
    
    // Attack 5: Configuration parameter corruption
    printf("--- Attack 5: Configuration Parameter Corruption ---\n");
    pump->attack_mode = ATTACK_CONFIG_PARAMS;
    pump->corruption_factor = 4;  // Corrupt calibration by factor of 4
    pump->mLBolus = 0.1;
    exec_time = measure_execution_time((void(*)(PumpState*))bolus_with_data_corruption, pump);
    printf("Execution time: %.2f ms\n", exec_time / 1000.0);
    print_status(pump);
    printf("\n");
    
    // Compound attack: Multiple corruptions
    printf("--- Compound Attack: Multiple Data Corruptions ---\n");
    pump->attack_mode = ATTACK_LOOP_COUNTER;  // Primary attack
    pump->corruption_factor = 2;
    pump->mLBolus = 2.0;  // Large dose
    pump->timing_corruption = 50;  // Also corrupt timing
    exec_time = measure_execution_time((void(*)(PumpState*))bolus_with_data_corruption, pump);
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