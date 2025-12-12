# LightFAt+ Syringe Pump Attack Variations Makefile
# Comprehensive build system for embedded system attestation validation
# Based on C-FLAT syringe pump implementation

CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c99
LDFLAGS = -lm

# Targets
TARGET_NORMAL = syringe_pump
TARGET_NODE_SKIP = attack_node_skipping
TARGET_ARBITRARY = attack_arbitrary_execution  
TARGET_DATA_CORRUPT = attack_data_corruption

# Sources
SOURCE_NORMAL = main.c
SOURCE_NODE_SKIP = attack_node_skipping.c
SOURCE_ARBITRARY = attack_arbitrary_execution.c
SOURCE_DATA_CORRUPT = attack_data_corruption.c

# All targets
TARGETS = $(TARGET_NORMAL) $(TARGET_NODE_SKIP) $(TARGET_ARBITRARY) $(TARGET_DATA_CORRUPT)

.PHONY: all clean test run attacks performance ml-dataset help validate

all: $(TARGETS)
	@echo "All attack variations built successfully!"
	@echo "Run 'make help' for usage instructions."

# Build individual targets
$(TARGET_NORMAL): $(SOURCE_NORMAL)
	$(CC) $(CFLAGS) -o $(TARGET_NORMAL) $(SOURCE_NORMAL) $(LDFLAGS)

$(TARGET_NODE_SKIP): $(SOURCE_NODE_SKIP)
	$(CC) $(CFLAGS) -o $(TARGET_NODE_SKIP) $(SOURCE_NODE_SKIP) $(LDFLAGS)

$(TARGET_ARBITRARY): $(SOURCE_ARBITRARY)
	$(CC) $(CFLAGS) -o $(TARGET_ARBITRARY) $(SOURCE_ARBITRARY) $(LDFLAGS)

$(TARGET_DATA_CORRUPT): $(SOURCE_DATA_CORRUPT)
	$(CC) $(CFLAGS) -o $(TARGET_DATA_CORRUPT) $(SOURCE_DATA_CORRUPT) $(LDFLAGS)

# Clean all targets
clean:
	rm -f $(TARGETS)

# Test normal simulator
test: $(TARGET_NORMAL)
	./$(TARGET_NORMAL)

# Run normal simulator
run: $(TARGET_NORMAL)
	./$(TARGET_NORMAL)

# Build and test all attack variations
attacks: $(TARGETS)
	@echo "=== Running Normal Baseline ==="
	./$(TARGET_NORMAL)
	@echo ""
	@echo "=== Running Node Skipping Attacks ==="
	./$(TARGET_NODE_SKIP)
	@echo ""
	@echo "=== Running Arbitrary Execution Attacks ==="
	./$(TARGET_ARBITRARY)
	@echo ""
	@echo "=== Running Data Corruption Attacks ==="
	./$(TARGET_DATA_CORRUPT)

# Validate all implementations work correctly
validate: $(TARGETS)
	@echo "=== Validation Suite ==="
	@echo "Testing normal operation..."
	@timeout 30s ./$(TARGET_NORMAL) > /dev/null && echo "✓ Normal pump: PASS" || echo "✗ Normal pump: FAIL"
	@echo "Testing node skipping attacks..."
	@timeout 30s ./$(TARGET_NODE_SKIP) > /dev/null && echo "✓ Node skipping: PASS" || echo "✗ Node skipping: FAIL"
	@echo "Testing arbitrary execution attacks..."
	@timeout 30s ./$(TARGET_ARBITRARY) > /dev/null && echo "✓ Arbitrary execution: PASS" || echo "✗ Arbitrary execution: FAIL"
	@echo "Testing data corruption attacks..."
	@timeout 30s ./$(TARGET_DATA_CORRUPT) > /dev/null && echo "✓ Data corruption: PASS" || echo "✗ Data corruption: FAIL"
	@echo "Testing random attack modes..."
	@timeout 10s ./$(TARGET_ARBITRARY) random > /dev/null && echo "✓ Random attacks: PASS" || echo "✗ Random attacks: FAIL"
	@timeout 10s ./$(TARGET_ARBITRARY) normal > /dev/null && echo "✓ Normal mode: PASS" || echo "✗ Normal mode: FAIL"
	@echo "Validation complete!"

# Generate ML training dataset
ml-dataset: $(TARGET_ARBITRARY)
	@echo "=== Generating Machine Learning Dataset ==="
	@rm -f attack_traces.log normal_traces.log dataset.csv
	@echo "Generating 100 attack traces..."
	@for i in $$(seq 1 100); do \
		echo "Attack,$$i" >> dataset.csv; \
		timeout 10s ./$(TARGET_ARBITRARY) random | grep "Execution time" >> dataset.csv 2>/dev/null || true; \
	done
	@echo "Generating 100 normal traces..."
	@for i in $$(seq 1 100); do \
		echo "Normal,$$i" >> dataset.csv; \
		timeout 10s ./$(TARGET_ARBITRARY) normal | grep "Execution time" >> dataset.csv 2>/dev/null || true; \
	done
	@echo "Dataset generated: dataset.csv (200 samples)"
	@wc -l dataset.csv

# Clean attack artifacts
clean-artifacts:
	@echo "Cleaning attack artifacts..."
	@rm -f /tmp/attack_log.txt /tmp/backdoor.sh /tmp/current_user.txt
	@rm -f /tmp/network_scan.txt /tmp/exfiltrated_logs.txt /tmp/ssh_processes.txt
	@rm -f /tmp/fake_mining_process /tmp/stolen_data.tar.gz
	@echo "Attack artifacts cleaned."

# Deep clean including dataset files
clean-all: clean clean-artifacts
	@rm -f dataset.csv attack_traces.log normal_traces.log
	@echo "Complete cleanup finished."

# Debug version with additional flags
debug: CFLAGS += -g -DDEBUG
debug: $(TARGETS)

# Performance testing - run all variations and capture timing
performance: $(TARGETS)
	@echo "=== Performance Comparison of Attack Variations ==="
	@echo "Baseline (Normal):"
	@time ./$(TARGET_NORMAL) > /dev/null
	@echo "Node Skipping Attacks:"
	@time ./$(TARGET_NODE_SKIP) > /dev/null
	@echo "Arbitrary Execution Attacks:"
	@time ./$(TARGET_ARBITRARY) > /dev/null
	@echo "Data Corruption Attacks:"
	@time ./$(TARGET_DATA_CORRUPT) > /dev/null

# Help target
help:
	@echo "=== LightFAt+ Syringe Pump Attack Variations ==="
	@echo ""
	@echo "Build targets:"
	@echo "  all         - Build all simulators (normal + attack variations)"
	@echo "  clean       - Remove compiled binaries"
	@echo "  clean-all   - Remove binaries, datasets, and attack artifacts"
	@echo "  debug       - Build all with debug symbols"
	@echo ""
	@echo "Testing targets:"
	@echo "  test        - Build and run normal simulator"
	@echo "  run         - Build and run normal simulator"  
	@echo "  attacks     - Build and run all attack demonstrations"
	@echo "  validate    - Run validation suite for all implementations"
	@echo "  performance - Time execution of all variations"
	@echo ""
	@echo "Machine Learning targets:"
	@echo "  ml-dataset  - Generate training dataset (200 samples)"
	@echo ""
	@echo "Individual attack targets:"
	@echo "  $(TARGET_NORMAL)           - Normal syringe pump simulator"
	@echo "  $(TARGET_NODE_SKIP)       - Node skipping attack variation"
	@echo "  $(TARGET_ARBITRARY)       - Arbitrary execution attack variation"
	@echo "  $(TARGET_DATA_CORRUPT)    - Data corruption attack variation"
	@echo ""
	@echo "Attack execution modes:"
	@echo "  ./$(TARGET_ARBITRARY)           - Demonstrate all attack modes"
	@echo "  ./$(TARGET_ARBITRARY) random    - Random attack for ML training"
	@echo "  ./$(TARGET_ARBITRARY) normal    - Normal operation baseline"
	@echo ""
	@echo "For detailed information, see README.md"