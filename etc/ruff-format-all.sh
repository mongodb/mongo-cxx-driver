#!/usr/bin/env bash
#
# format.sh
#
# Usage:
#   uv run --frozen etc/ruff-format-all.sh
#
# This script is meant to be run from the project root directory.

set -o errexit
set -o pipefail

# Python scripts.
# https://github.com/astral-sh/ruff/issues/8232
uv run --frozen --group format-scripts ruff check --select I --fix
uv run --frozen --group format-scripts ruff format
