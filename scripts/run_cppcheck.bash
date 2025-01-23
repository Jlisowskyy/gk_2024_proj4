#!/bin/bash

RUN_CPPCHECK_DIR="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
RUN_CPPCHECK_REPORTS_DIR="${RUN_CPPCHECK_DIR}/../reports"
RUN_CPPCHECK_SRC_DIR="${RUN_CPPCHECK_DIR}/../src"
RUN_CPPCHECK_COMPILE_DB_DIR="${RUN_CPPCHECK_DIR}/../compile_commands.json"

# Step 1: Generate compile_commands.json
"${RUN_CPPCHECK_DIR}/generate_compile_db.bash"

# Step 2: Run cppcheck
mkdir -p "${RUN_CPPCHECK_REPORTS_DIR}"
cppcheck --enable=all --suppress=missingIncludeSystem --error-exitcode=1 \
    --checkers-report="${RUN_CPPCHECK_REPORTS_DIR}"/checkers-report.txt \
    --output-file="${RUN_CPPCHECK_REPORTS_DIR}"/cppcheck-output.txt \
    --project="${RUN_CPPCHECK_COMPILE_DB_DIR}"
