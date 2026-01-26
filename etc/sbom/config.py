#!/usr/bin/env python3
"""generate_sbom.py config. Operational configuration values stored separately from the core code."""

import logging
import re
import subprocess

import semver

logger = logging.getLogger('generate_sbom')
logger.setLevel(logging.NOTSET)

# ################ Component Filters ################

# List of Endor Labs SBOM components that must be removed before processing
components_remove = [
    # A dependency erroneously matched in build/CMakeFiles
    'mozilla/cubeb',
    # An incorrect match from parts of pkg:github/madler/zlib
    'zlib-ng/zlib-ng',
]

# bom-ref prefixes (Endor Labs has been changing them, so add all that we have seen)
prefixes = [
    'pkg:c/github.com/',
    'pkg:generic/github.com/',
    'pkg:github/',
]

endor_components_remove = []
for component in components_remove:
    for prefix in prefixes:
        endor_components_remove.append(prefix + component)

# ################ Component Renaming ################
# Endor does not have syntactically valid PURLs for C/C++ packages.
# e.g.,
# Invalid: pkg:c/github.com/abseil/abseil-cpp@20250512.1
# Valid: pkg:github/abseil/abseil-cpp@20250512.1
# Run string replacements to correct for this:
endor_components_rename = [
    ['pkg:generic/zlib.net/zlib', 'pkg:github/madler/zlib'],
    # in case of regression
    ['pkg:generic/github.com/', 'pkg:github/'],
    ['pkg:c/github.com/', 'pkg:github/'],
]


# ################ Primary Component Version ################
def run_repo_version_script() -> str:
    """Attempt to determine primary component version using repo script."""

    # mongo-cxx-driver: etc/calc_release_version.py
    try:
        result = subprocess.run(['python', 'etc/calc_release_version.py'], capture_output=True, text=True)
        version = semver.VersionInfo.parse(result.stdout)
        if version.match('0.0.0'):
            return None
        else:
            return version
    except Exception as e:
        logger.warning(
            'PRIMARY COMPONENT VERSION: Unable to parse output from etc/calc_release_version.py: %s', result.stdout
        )
        logger.warning(e)
        return None


# ################ Version Transformation ################

# In some cases we need to transform the version string to strip out tag-related text
# It is unknown what patterns may appear in the future, so we have targeted (not broad) regex
# This a list of 'pattern' and 'repl' inputs to re.sub()
RE_VER_NUM = r'(0|[1-9]\d*)'
RE_VER_LBL = r'(?:-((?:0|[1-9]\d*|\d*[a-zA-Z-][0-9a-zA-Z-]*)(?:\.(?:0|[1-9]\d*|\d*[a-zA-Z-][0-9a-zA-Z-]*))*))?(?:\+([0-9a-zA-Z-]+(?:\.[0-9a-zA-Z-]+)*))?'
RE_SEMVER = rf'{RE_VER_NUM}\.{RE_VER_NUM}\.{RE_VER_NUM}{RE_VER_LBL}'
regex_semver = re.compile(RE_SEMVER)

# Release Naming Conventions
REGEX_RELEASE_BRANCH = rf'^releases/v(.*)$'  # e.g., releases/v4.1
REGEX_RELEASE_TAG = rf'^(r{RE_SEMVER})|(debian/{RE_SEMVER}-1)$'  # e.g., r3.7.0-beta1, debian/4.1.4-1

VERSION_PATTERN_REPL = [
    # 'debian/1.28.1-1' pkg:github/mongodb/mongo-c-driver (temporary workaround)
    [re.compile(rf'^debian/({RE_SEMVER})-\d$'), r'\1'],
    # 'gperftools-2.9.1' pkg:github/gperftools/gperftools
    # 'mongo/v1.5.2' pkg:github/google/benchmark
    # 'mongodb-8.2.0-alpha2' pkg:github/wiredtiger/wiredtiger
    # 'release-1.12.0' pkg:github/apache/avro
    # 'yaml-cpp-0.6.3' pkg:github/jbeder/yaml-cpp
    [re.compile(rf'^[-a-z]+[-/][vr]?({RE_SEMVER})$'), r'\1'],
    # 'asio-1-34-2' pkg:github/chriskohlhoff/asio
    # 'cares-1_27_0' pkg:github/c-ares/c-ares
    [
        re.compile(rf'^[a-z]+-{RE_VER_NUM}[_-]{RE_VER_NUM}[_-]{RE_VER_NUM}{RE_VER_LBL}$'),
        r'\1.\2.\3',
    ],
    # 'pcre2-10.40' pkg:github/pcre2project/pcre2
    [re.compile(rf'^[a-z0-9]+-({RE_VER_NUM}\.{RE_VER_NUM})$'), r'\1'],
    # 'icu-release-57-1' pkg:github/unicode-org/icu
    [re.compile(rf'^[a-z]+-?[a-z]+-{RE_VER_NUM}-{RE_VER_NUM}$'), r'\1.\2'],
    # 'v2.6.0'  pkg:github/confluentinc/librdkafka
    # 'r2.5.1'
    [re.compile(rf'^[rv]({RE_SEMVER})$'), r'\1'],
    # 'v2025.04.21.00' pkg:github/facebook/folly
    [re.compile(r'^v(\d+\.\d+\.\d+\.\d+)$'), r'\1'],
]


def get_semver_from_release_version(release_ver: str) -> semver:
    """Extract the version number from string with tags or other annotations"""
    if release_ver:
        for re_obj, repl in VERSION_PATTERN_REPL:
            if re_obj.match(release_ver):
                return re_obj.sub(repl, release_ver)
    return release_ver


# region special component use-case functions


def process_component_special_cases(component_key: str, component: dict, versions: dict, repo_root: str) -> None:
    pass


# endregion special component use-case functions
