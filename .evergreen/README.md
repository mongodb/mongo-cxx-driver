# Evergreen Config Generation

## Generation

Use [Astral uv](https://docs.astral.sh/uv/) to run the `config_generator/generate.py` script from the project root directory:

```bash
uv run .evergreen/config_generator/generate.py
```

Python binary and package requirements are defined in the project root directory's `pyproject.toml` file.

## Layout

The contents of this directory are organized as follows:

- `config.yml`: the root Evergreen config file.
- `generated_configs`: generated Evergreen config files included by `config.yml`.
- `config_generator`: Python scripts used to generate config files under `generated_configs`.
- `scripts`: shell scripts used by the generated Evergreen config.

## Config Generator

Config generator scripts are organized into three subdirectories.

### Components

These scripts define Evergreen functions, tasks, task groups, and build variants. Components which only define Evergreen functions (for reuse by multiple components) are grouped under the `funcs` subdirectory. All other components (which define a task, task group, or build variant) are located outside the `funcs` directory.

### Etc

These scripts define helper utilities used by components, but do not define any Evergreen functions, tasks, task groups, or build variants themselves. These scripts are only imported by scripts under `components`.

### Generators

These scripts are imported by `generate.py` and are each responsible for generating an Evergreen config file under `generated_configs` (`functions.py` generates `functions.yml`, etc.). These scripts only scan the contents of the `components` directory.
