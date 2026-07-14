#!/usr/bin/env bash

export_uv_tool_dirs() {
  : "${UV_TOOL_DIR:="$(mktemp -d)"}" || return
  : "${UV_TOOL_BIN_DIR:="$(mktemp -d)"}" || return

  PATH="${UV_TOOL_BIN_DIR:?}:${PATH:-}"

  # Windows requires "C:\path\to\dir" instead of "/cygdrive/c/path/to/dir" (PATH is automatically converted).
  if [[ "${OSTYPE:?}" == cygwin ]]; then
    UV_TOOL_DIR="$(cygpath -aw "${UV_TOOL_DIR:?}")" || return
    UV_TOOL_BIN_DIR="$(cygpath -aw "${UV_TOOL_BIN_DIR:?}")" || return
  fi

  UV_PYTHON_INSTALL_DIR="${UV_TOOL_DIR:?}"

  export PATH UV_TOOL_DIR UV_TOOL_BIN_DIR UV_PYTHON_INSTALL_DIR
}

install_build_tools() {
  export_uv_tool_dirs || return

  # PyPI `cmake` requires a sufficiently recent Python version.
  uv python install --no-bin -q || uv python install -q || return

  # TODO: replace "cmake==4.3.4" with "cmake" once the C driver dependency is updated to 2.3.3 to include CDRIVER-6367.
  uv tool install -q "cmake==4.3.4" || return

  if [[ -f /etc/redhat-release && -x /opt/mongodbtoolchain/v4/bin/ninja ]]; then
    # Avoid strange "Could NOT find Threads" CMake configuration error on RHEL when using PyPI CMake, PyPI Ninja, and
    # C++20 or newer by using MongoDB Toolchain's Ninja binary instead.
    ln -sf /opt/mongodbtoolchain/v4/bin/ninja "${UV_TOOL_BIN_DIR:?}/ninja" || return
  else
    uv tool install -q ninja || return
  fi

  # Ensure `pkg-config` provided by PyPI pkgconf is able to fallback to the system `pkg-config`.
  export FORCE_PKGCONF_PYPI=1
  uv tool install -q pkgconf || return

  uvx python --version || return
  cmake --version | head -n 1 || return
  echo "ninja version: $(ninja --version)" || return
  echo "pkgconf version: $(pkgconf --version 2>/dev/null)" || return

  if [[ "${OSTYPE:?}" != "cygwin" ]]; then
    export CMAKE_GENERATOR="${CMAKE_GENERATOR:="Ninja"}"
  fi
}
