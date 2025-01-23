#!/bin/bash

BUILD_AND_RUN_UNIT_TESTS_DIR="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

BUILD_AND_RUN_UNIT_TESTS_REPO_DIR="${BUILD_AND_RUN_UNIT_TESTS_DIR}/.."

TEST_TARGET_NAME="UnitTestTarget"

set -euo pipefail  # Exit on error, unset variables, and pipeline failures

# Step 1: cd to the repo root
cd "${BUILD_AND_RUN_UNIT_TESTS_REPO_DIR}" || exit 1

# Step 2: configure cmake

mkdir -p build && cd build || exit 1
cmake -DCMAKE_BUILD_TYPE=Debug .. || exit 1

# Step 3: build the project (additionally verify if everything builds correctly)
cmake --build . -- -j$(nproc) || exit 1

# Step 5: find test binary
TEST_BINARY=$(find . -name "${TEST_TARGET_NAME}" -type f)

# Step 6: run tests
"${TEST_BINARY}" || exit 1
