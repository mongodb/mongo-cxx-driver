import textwrap
import unittest

import click

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


class TestMakeRelease(unittest.TestCase):
    def test_success(self):
        got = make_release.generate_release_notes('3.9.0', CHANGELOG)
        self.assertEqual(got, EXPECTED_RELEASE_NOTES)

    def test_raises_when_version_not_found(self):
        with self.assertRaises(click.ClickException) as ctx:
            make_release.generate_release_notes('4.0.0', CHANGELOG)
        self.assertIn('Failed to find', str(ctx.exception.message))

    def test_raises_on_unexpected_extra_characters(self):
        with self.assertRaises(click.ClickException) as ctx:
            make_release.generate_release_notes('3.9.0', CHANGELOG_UNRELEASED)
        self.assertIn('Unexpected extra characters', str(ctx.exception.message))

    def test_allows_unreleased_tag(self):
        got = make_release.generate_release_notes('3.9.0', CHANGELOG_UNRELEASED, allow_unreleased_changelog_entry=True)
        self.assertEqual(got, EXPECTED_RELEASE_NOTES)

    def test_allows_unreleased_tag_only(self):
        with self.assertRaises(click.ClickException) as ctx:
            make_release.generate_release_notes(
                '3.9.0', make_changelog(' extra'), allow_unreleased_changelog_entry=True
            )
        self.assertIn('Unexpected extra characters', str(ctx.exception.message))

    def test_raises_on_duplicate_version_entry(self):
        with self.assertRaises(click.ClickException) as ctx:
            make_release.generate_release_notes('3.9.0', CHANGELOG_DUPLICATE_VERSION)
        self.assertIn('Unexpected second changelog entry', str(ctx.exception.message))


if __name__ == '__main__':
    unittest.main()
