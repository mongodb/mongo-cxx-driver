import sys
import textwrap
import types

import click
import pytest

import make_release

# Changelog entries for the 1.2.3 release.
CHANGELOG_ENTRIES = textwrap.dedent("""\
- Add CMake option `MONGOCXX_OVERRIDE_DEFAULT_INSTALL_PREFIX` (default is `TRUE`
    for backwards-compatibility).
- Add API to manage Atlas Search Indexes.
""")


# The expected release notes generated for the 1.2.3 release.
EXPECTED_RELEASE_NOTES = f"""\
## Added

{CHANGELOG_ENTRIES}
See the [full list of changes in Jira](https://jira.mongodb.org/issues/?jql=project%20%3D%20CXX%20AND%20fixVersion%20%3D%201.2.3).

## Feedback
To report a bug or request a feature, please open a ticket in the MongoDB issue management tool Jira:

- [Create an account](https://jira.mongodb.org) and login.
- Navigate to the [CXX project](https://jira.mongodb.org/browse/CXX)
- Click `Create`.

## Signature Verification
Release artifacts may be verified by using the accompanying detached signature (.asc) and the cpp-driver public key obtained from https://pgp.mongodb.com.
"""


# The CHANGELOG.md file containing multiple release entries, including the 1.2.3 release.
def make_changelog(version_suffix: str) -> str:
    return f"""\
# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## 1.2.3{version_suffix}

### Added

{CHANGELOG_ENTRIES}
## 3.8.0

### Fixed

- Fix foo.
"""


# The official 1.2.3 changelog.
CHANGELOG = make_changelog('')

# The unreleased 1.2.3 changelog.
CHANGELOG_UNRELEASED = make_changelog(' [Unreleased]')

# Duplicate release entry check.
CHANGELOG_DUPLICATE_VERSION = textwrap.dedent("""
## 1.2.3

### Added

- Add something.

## 1.2.3

### Fixed

- Fix something.

## 3.8.0

### Fixed

- Fix foo.
""").lstrip()


class TestGenerateReleaseNotes:
    fn = staticmethod(make_release.generate_release_notes)

    def test_success(self):
        got = self.fn('1.2.3', CHANGELOG)
        assert got == EXPECTED_RELEASE_NOTES

    def test_missing(self):
        with pytest.raises(click.ClickException, match='Failed to find'):
            self.fn('2.0.0', CHANGELOG)

    def test_extra(self):
        with pytest.raises(click.ClickException, match='Unexpected extra characters'):
            self.fn('1.2.3', CHANGELOG_UNRELEASED)

    def test_allow_unreleased_changelog_entry(self):
        got = self.fn('1.2.3', CHANGELOG_UNRELEASED, allow_unreleased_changelog_entry=True)
        assert got == EXPECTED_RELEASE_NOTES

    def test_allows_unreleased_changelog_entry_only(self):
        with pytest.raises(click.ClickException, match='Unexpected extra characters'):
            self.fn('1.2.3', make_changelog(' extra'), allow_unreleased_changelog_entry=True)

    def test_duplicate(self):
        with pytest.raises(click.ClickException, match='Unexpected second changelog entry'):
            self.fn('1.2.3', CHANGELOG_DUPLICATE_VERSION)


class TestIsPreReleaseTag:
    fn = staticmethod(make_release.is_pre_release_tag)

    @pytest.mark.parametrize(
        'tag',
        [
            'r1.2.3-rc0',
            'r1.2.3-alpha',
            'r1.2.3-extra',
            'r1.2.3-a.b.c',
        ],
    )
    def test_is_pre_release(self, tag):
        assert self.fn(tag)

    @pytest.mark.parametrize(
        'tag',
        [
            'r0.0.0',
            'r1.2.3',
            'r1.2.4',
            'r2.0.0',
        ],
    )
    def test_normal(self, tag):
        assert not self.fn(tag)

    @pytest.mark.parametrize(
        'tag',
        [
            'invalid',
            'r1.2.3-',
            'r1.2.3.invalid',
            'r1.2.3invalid',
        ],
    )
    def test_invalid(self, tag):
        assert not self.fn(tag)


class TestPrintBanner:
    fn = staticmethod(make_release.print_banner)

    @pytest.mark.parametrize(
        'tag',
        [
            'invalid',
            'r1.2.3',
            'r2.0.0',
        ],
    )
    def test_contains_git_revision(self, tag, capsys):
        self.fn(tag)
        captured = capsys.readouterr()
        assert f'This build is for Git revision {tag}' in captured.out


def _make_issue(key, status_name):
    ret = types.SimpleNamespace()

    ret.key = key

    ret.fields = types.SimpleNamespace()
    ret.fields.status = types.SimpleNamespace()
    ret.fields.status.name = status_name

    return ret


class TestAllIssuesClosed:
    fn = staticmethod(make_release.all_issues_closed)

    @pytest.mark.parametrize(
        'issues',
        [
            [],
            [_make_issue('CXX-1234', 'Closed')],
            [
                _make_issue('CXX-1234', 'Closed'),
                _make_issue('CXX-2345', 'Development Complete'),
            ],
            [
                _make_issue('CXX-2345', 'Development Complete'),
                _make_issue('CXX-1234', 'Closed'),
            ],
            [
                _make_issue('CXX-1234', 'Closed'),
                _make_issue('CXX-2345', 'Development Complete'),
                _make_issue('CXX-3456', 'Closed'),
            ],
        ],
    )
    def test_all_closed(self, issues, capsys):
        result = self.fn(issues)
        assert result

        captured = capsys.readouterr()
        assert captured.out == ''
        assert captured.err == ''

    @pytest.mark.parametrize(
        'issues, expected',
        [
            (
                [_make_issue('CXX-1234', 'Open')],
                ['CXX-1234'],
            ),
            (
                [
                    _make_issue('CXX-1234', 'Backlog'),
                    _make_issue('CXX-2345', 'Closed'),
                ],
                ['CXX-1234'],
            ),
            (
                [
                    _make_issue('CXX-2345', 'Closed'),
                    _make_issue('CXX-1234', 'In Progress'),
                ],
                ['CXX-1234'],
            ),
            (
                [
                    _make_issue('CXX-1234', 'In Code Review'),
                    _make_issue('CXX-2345', 'Closed'),
                    _make_issue('CXX-3456', 'Needs Triage'),
                    _make_issue('CXX-4567', 'Closed'),
                    _make_issue('CXX-5678', 'Ready for Work'),
                ],
                ['CXX-1234', 'CXX-3456', 'CXX-5678'],
            ),
        ],
    )
    def test_any_open(self, issues, expected, capsys):
        result = self.fn(issues)
        assert not result

        capture = capsys.readouterr()
        assert capture.out == ''

        err = capture.err
        assert 'The following open tickets were found:' in err

        for issue in expected:
            assert issue in err

        for issue in {i.key for i in issues} - set(expected):
            assert issue not in err


def main(argv=()):
    sys.exit(pytest.main([__file__, *argv]))


if __name__ == '__main__':
    main(sys.argv[1:])
