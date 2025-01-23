#!/bin/bash

GENERATE_COMPILE_DB_DIR="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
GENERATE_COMPILE_DB_REPO_DIR="${GENERATE_COMPILE_DB_DIR}/.."
REPO_SRC_DIR="/home/Jlisowskyy/Storage/Repos/gk_2024_proj4/src"

fail() {
  echo "Compile DB generation failed"
  exit 1
}

set -euo pipefail  # Exit on error, unset variables, and pipeline failures

# Step 1: cd to the repo root
cd "${GENERATE_COMPILE_DB_REPO_DIR}" || fail

# Step 2: generate compile_commands.json
mkdir -p build && cd build || fail
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Debug .. || fail

# Step 3. Find compile_commands.json
COMPILE_COMMANDS_JSON=$(find . -name "compile_commands.json" -type f)

# Step 4: copy original compile_commands.json to the repo root
cp "${COMPILE_COMMANDS_JSON}" "${GENERATE_COMPILE_DB_REPO_DIR}/original_compile_commands.json" || fail

# Step 5: Filter compile_commands.json to keep only files from src directory
jq --arg src_dir "${REPO_SRC_DIR}" '[.[] | select(.file | startswith($src_dir))]' \
    "${GENERATE_COMPILE_DB_REPO_DIR}/original_compile_commands.json" \
    > "${GENERATE_COMPILE_DB_REPO_DIR}/compile_commands.json" || fail

# Step 6: cleanup
rm "${GENERATE_COMPILE_DB_REPO_DIR}/original_compile_commands.json" || fail
