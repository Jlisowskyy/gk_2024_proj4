#!/bin/bash

SETUP_DEV_ENV_DIR="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

# ------------------------------
# Install C++ dev packages
# ------------------------------

# TODO: define for the project

# ------------------------------
# Setup git hooks
# ------------------------------

SETUP_DEV_ENV_GIT_HOOKS_SCRIP="${SETUP_DEV_ENV_DIR}/git-hooks/setup-hooks.bash"
${SETUP_DEV_ENV_GIT_HOOKS_SCRIP}

# ------------------------------
# Load custom scripts
# ------------------------------

# TODD:
