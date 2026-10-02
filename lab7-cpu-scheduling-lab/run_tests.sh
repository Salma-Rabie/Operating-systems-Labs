#!/usr/bin/env bash

# --------------------------------------------------
# Lab 7 – CPU Scheduling Local Test Runner
# Mirrors GitHub Classroom autograding workflow
# --------------------------------------------------

set -e

LAB_EXEC=./lab7
TEST_DIR=testcases

# ---------- BUILD ----------
echo "🔨 Building lab7..."
make
chmod +x lab7

echo "✅ Build successful"
echo

# ---------- COLORS ----------
GREEN="\e[32m"
RED="\e[31m"
YELLOW="\e[33m"
RESET="\e[0m"

# ---------- COUNTERS ----------
TOTAL=0
PASSED=0

# ---------- FUNCTION ----------
run_test() {
    local test_name=$1
    local input_file=$2
    local output_file=$3

    TOTAL=$((TOTAL+1))

    echo -n "Running $test_name ... "

    if cat "$input_file" | $LAB_EXEC | diff "$output_file" - > /dev/null; then
        echo -e "${GREEN}PASS${RESET}"
        PASSED=$((PASSED+1))
    else
        echo -e "${RED}FAIL${RESET}"
        echo -e "${YELLOW}Diff:${RESET}"
        cat "$input_file" | $LAB_EXEC | diff "$output_file" -
        echo
    fi
}

# ---------- TESTS ----------

# 01
run_test "01a" $TEST_DIR/01a-input.txt $TEST_DIR/01a-output.txt
run_test "01b" $TEST_DIR/01b-input.txt $TEST_DIR/01b-output.txt
run_test "01c" $TEST_DIR/01c-input.txt $TEST_DIR/01c-output.txt
run_test "01d" $TEST_DIR/01d-input.txt $TEST_DIR/01d-output.txt

# 02
run_test "02a" $TEST_DIR/02a-input.txt $TEST_DIR/02a-output.txt
run_test "02b" $TEST_DIR/02b-input.txt $TEST_DIR/02b-output.txt
run_test "02c" $TEST_DIR/02c-input.txt $TEST_DIR/02c-output.txt
run_test "02d" $TEST_DIR/02d-input.txt $TEST_DIR/02d-output.txt

# 03
run_test "03a" $TEST_DIR/03a-input.txt $TEST_DIR/03a-output.txt
run_test "03b" $TEST_DIR/03b-input.txt $TEST_DIR/03b-output.txt
run_test "03c" $TEST_DIR/03c-input.txt $TEST_DIR/03c-output.txt
run_test "03d" $TEST_DIR/03d-input.txt $TEST_DIR/03d-output.txt

# 04
run_test "04a" $TEST_DIR/04a-input.txt $TEST_DIR/04a-output.txt
run_test "04b" $TEST_DIR/04b-input.txt $TEST_DIR/04b-output.txt
run_test "04c" $TEST_DIR/04c-input.txt $TEST_DIR/04c-output.txt
run_test "04d" $TEST_DIR/04d-input.txt $TEST_DIR/04d-output.txt

# 05
run_test "05a" $TEST_DIR/05a-input.txt $TEST_DIR/05a-output.txt
run_test "05b" $TEST_DIR/05b-input.txt $TEST_DIR/05b-output.txt
run_test "05c" $TEST_DIR/05c-input.txt $TEST_DIR/05c-output.txt
run_test "05d" $TEST_DIR/05d-input.txt $TEST_DIR/05d-output.txt

# 06
run_test "06a" $TEST_DIR/06a-input.txt $TEST_DIR/06a-output.txt
run_test "06b" $TEST_DIR/06b-input.txt $TEST_DIR/06b-output.txt
run_test "06c" $TEST_DIR/06c-input.txt $TEST_DIR/06c-output.txt
run_test "06d" $TEST_DIR/06d-input.txt $TEST_DIR/06d-output.txt

# 07
run_test "07a" $TEST_DIR/07a-input.txt $TEST_DIR/07a-output.txt
run_test "07b" $TEST_DIR/07b-input.txt $TEST_DIR/07b-output.txt
run_test "07c" $TEST_DIR/07c-input.txt $TEST_DIR/07c-output.txt
run_test "07d" $TEST_DIR/07d-input.txt $TEST_DIR/07d-output.txt

# 08
run_test "08a" $TEST_DIR/08a-input.txt $TEST_DIR/08a-output.txt
run_test "08b" $TEST_DIR/08b-input.txt $TEST_DIR/08b-output.txt

# 09
run_test "09a" $TEST_DIR/09a-input.txt $TEST_DIR/09a-output.txt
run_test "09b" $TEST_DIR/09b-input.txt $TEST_DIR/09b-output.txt

# 10
run_test "10a" $TEST_DIR/10a-input.txt $TEST_DIR/10a-output.txt
run_test "10b" $TEST_DIR/10b-input.txt $TEST_DIR/10b-output.txt

# 11
run_test "11a" $TEST_DIR/11a-input.txt $TEST_DIR/11a-output.txt

# 12
run_test "12a" $TEST_DIR/12a-input.txt $TEST_DIR/12a-output.txt

# ---------- SUMMARY ----------
echo
echo "======================================"
echo -e "✅ Passed: ${GREEN}$PASSED${RESET} / $TOTAL"
echo "======================================"
