import sys
import textwrap

import click
import pytest

import make_release

# Changelog entries for the 3.9.0 release.
CHANGELOG_ENTRIES = textwrap.dedent("""\
- Add CMake option `MONGOCXX_OVERRIDE_DEFAULT_INSTALL_PREFIX` (default is `TRUE`
    for backwards-compatibility).
- Add API to manage Atlas Search Indexes.
""")


# The expected release notes generated for the 3.9.0 release.
EXPECTED_RELEASE_NOTES = f"""\
## Added

{CHANGELOG_ENTRIES}
See the [full list of changes in Jira](https://jira.mongodb.org/issues/?jql=project%20%3D%20CXX%20AND%20fixVersion%20%3D%203.9.0).

## Feedback
To report a bug or request a feature, please open a ticket in the MongoDB issue management tool Jira:

- [Create an account](https://jira.mongodb.org) and login.
- Navigate to the [CXX project](https://jira.mongodb.org/browse/CXX)
- Click `Create`.

## Signature Verification
Release artifacts may be verified by using the accompanying detached signature (.asc) and the cpp-driver public key obtained from https://pgp.mongodb.com.
"""


# The CHANGELOG.md file containing multiple release entries, including the 3.9.0 release.
def make_changelog(version_suffix: str) -> str:
    return f"""\
# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## 3.9.0{version_suffix}

### Added

{CHANGELOG_ENTRIES}
## 3.8.0

### Fixed

- Fix foo.
"""


# The official 3.9.0 changelog.
CHANGELOG = make_changelog('')

# The unreleased 3.9.0 changelog.
CHANGELOG_UNRELEASED = make_changelog(' [Unreleased]')

# Duplicate release entry check.
CHANGELOG_DUPLICATE_VERSION = textwrap.dedent("""
## 3.9.0

### Added

- Add something.

## 3.9.0

### Fixed

- Fix something.

## 3.8.0

### Fixed

- Fix foo.
""").lstrip()


class TestGenerateReleaseNotes:
    fn = staticmethod(make_release.generate_release_notes)

    def test_success(self):
        got = self.fn('3.9.0', CHANGELOG)
        assert got == EXPECTED_RELEASE_NOTES

    def test_missing(self):
        with pytest.raises(click.ClickException, match='Failed to find'):
            self.fn('4.0.0', CHANGELOG)

    def test_extra(self):
        with pytest.raises(click.ClickException, match='Unexpected extra characters'):
            self.fn('3.9.0', CHANGELOG_UNRELEASED)

    def test_allow_unreleased_changelog_entry(self):
        got = self.fn('3.9.0', CHANGELOG_UNRELEASED, allow_unreleased_changelog_entry=True)
        assert got == EXPECTED_RELEASE_NOTES

    def test_allows_unreleased_changelog_entry_only(self):
        with pytest.raises(click.ClickException, match='Unexpected extra characters'):
            self.fn('3.9.0', make_changelog(' extra'), allow_unreleased_changelog_entry=True)

    def test_duplicate(self):
        with pytest.raises(click.ClickException, match='Unexpected second changelog entry'):
            self.fn('3.9.0', CHANGELOG_DUPLICATE_VERSION)


def main(argv=()):
    sys.exit(pytest.main([__file__, *argv]))


if __name__ == '__main__':
    main(sys.argv[1:])
