/*
 * Syringe Pump Simulator - C Implementation
 * 
 * This simulates the behavior of an open-source syringe pump
 * without requiring actual Arduino hardware. It mimics the timing-critical
 * motor control loops and calculations from the original Arduino code.
 * 
 * Based on: https://github.com/manimino/OpenSyringePump
 * 
 * Compile with: gcc -o syringe_pump_sim syringe_pump_sim.c -lm
 * Run with: ./syringe_pump_sim
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
} PumpState;

/* -- Function prototypes -- */
void init_pump(PumpState *pump);
void bolus(PumpState *pump, Direction direction);
void simulate_motor_step(long steps, int speed_us);
void print_status(PumpState *pump);
void run_test_sequence(PumpState *pump);
double get_time_us(void);

/* -- Main -- */
int main(void) {
    PumpState pump;
    
    printf("=== Syringe Pump Simulator ===\n");
    printf("Based on OpenSyringePump Arduino project\n");
    printf("Simulating motor control and timing loops\n\n");
    
    init_pump(&pump);
    print_status(&pump);
    
    // Run a test sequence that mimics real usage
    run_test_sequence(&pump);
    
    printf("\n=== Simulation Complete ===\n");
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
    
    printf("Pump initialized:\n");
    printf("  Steps per mm: %ld\n", pump->ustepsPerMM);
    printf("  Steps per mL: %ld\n", pump->ustepsPerML);
    printf("  Default bolus: %.3f mL\n\n", pump->mLBolus);
}

/* Simulate a bolus push or pull */
void bolus(PumpState *pump, Direction direction) {
    long steps = (long)(pump->mLBolus * pump->ustepsPerML);
    
    if (direction == PUSH) {
        printf("PUSH: Dispensing %.3f mL (%ld steps)...", pump->mLBolus, steps);
        fflush(stdout);
        
        simulate_motor_step(steps, SPEED_MICROSECONDS_DELAY);
        
        pump->mLUsed += pump->mLBolus;
        pump->stepperPos += steps;
    } 
    else if (direction == PULL) {
        printf("PULL: Retracting %.3f mL (%ld steps)...", pump->mLBolus, steps);
        fflush(stdout);
        
        simulate_motor_step(steps, SPEED_MICROSECONDS_DELAY);
        
        if ((pump->mLUsed - pump->mLBolus) > 0) {
            pump->mLUsed -= pump->mLBolus;
        } else {
            pump->mLUsed = 0.0;
        }
        pump->stepperPos -= steps;
    }
    
    printf(" Done!\n");
}

/* Simulate motor stepping with precise timing */
void simulate_motor_step(long steps, int speed_us) {
    struct timespec delay;
    delay.tv_sec = 0;
    delay.tv_nsec = speed_us * 1000;  // convert microseconds to nanoseconds
    
    // This loop mimics the timing-critical motor control
    // from the Arduino code's bolus() function
    for (long i = 0; i < steps; i++) {
        // Simulate HIGH pulse
        nanosleep(&delay, NULL);
        
        // Simulate LOW pulse  
        nanosleep(&delay, NULL);
        
        // Add some computational load every so often
        if (i % 100 == 0) {
            // Mimic the calculations that happen in real motor control
            volatile double result = sin((double)i / 100.0) * cos((double)i / 50.0);
            result = result * result;  // prevent optimization
        }
    }
}

/* Print current pump status */
void print_status(PumpState *pump) {
    printf("Status: Used %.3f mL, Position %ld steps, Bolus size %.3f mL\n", 
           pump->mLUsed, pump->stepperPos, pump->mLBolus);
}

/* Run a test sequence that mimics realistic usage */
void run_test_sequence(PumpState *pump) {
    printf("=== Running Test Sequence ===\n\n");
    
    // Test 1: Small boluses
    printf("Test 1: Small precision doses\n");
    pump->mLBolus = 0.01;
    for (int i = 0; i < 5; i++) {
        bolus(pump, PUSH);
        print_status(pump);
    }
    printf("\n");
    
    // Test 2: Medium boluses
    printf("Test 2: Medium doses\n");
    pump->mLBolus = 0.5;
    for (int i = 0; i < 3; i++) {
        bolus(pump, PUSH);
        print_status(pump);
    }
    printf("\n");
    
    // Test 3: Large bolus
    printf("Test 3: Large dose\n");
    pump->mLBolus = 2.0;
    bolus(pump, PUSH);
    print_status(pump);
    printf("\n");
    
    // Test 4: Refill (pull back)
    printf("Test 4: Refill sequence\n");
    pump->mLBolus = 1.0;
    for (int i = 0; i < 3; i++) {
        bolus(pump, PULL);
        print_status(pump);
    }
    printf("\n");
    
    // Test 5: Variable speed sequence
    printf("Test 5: Variable dosing pattern\n");
    float doses[] = {0.05, 0.1, 0.2, 0.5, 1.0};
    for (int i = 0; i < 5; i++) {
        pump->mLBolus = doses[i];
        bolus(pump, PUSH);
        print_status(pump);
    }
    printf("\n");
    
    // Test 6: Stress test - many small rapid doses
    printf("Test 6: Rapid dosing (stress test)\n");
    pump->mLBolus = 0.001;
    printf("Performing 100 micro-doses...\n");
    for (int i = 0; i < 100; i++) {
        bolus(pump, PUSH);
        if (i % 25 == 0) {
            print_status(pump);
        }
    }
    printf("\n");
}

/* Get current time in microseconds */
double get_time_us(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec * 1000000.0 + (double)ts.tv_nsec / 1000.0;
}