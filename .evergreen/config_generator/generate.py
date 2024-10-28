#!/usr/bin/env python3

# Generates *.yml files under .evergreen/generated_configs.

# /// script
# requires-python = ">=3.10"
# dependencies = [
#   "packaging>=14.0",
#   "pydantic>=2.7",
#   "shrub-py>=3.3.1",
# ]
# ///

import os
import sys

from importlib import import_module


GENERATOR_NAMES = [
    "functions",
    "tasks",
    "task_groups",
    "variants",
]


def main():
    # Permit `import config_generator.*`.
    sys.path.append(os.path.join(os.path.dirname(__file__), '..'))

    for name in GENERATOR_NAMES:
        m = import_module(f"config_generator.generators.{name}")
        print(f"Running {name}.generate()...")
        m.generate()
        print(f"Running {name}.generate()... done.")


if __name__ == "__main__":
    main()
