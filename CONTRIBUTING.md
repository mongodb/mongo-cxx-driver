# Contributing Guidelines

Follow the [MongoDB C++ Driver Coding Guidelines](https://github.com/mongodb/mongo-cxx-driver/etc/coding_guidelines.md), ensure files are formatted properly, and follow guidelines for writing PR titles and commit messages.

All contributions must be made via a GitHub pull request (PR).

## Commit Messages

Prefix the PR title with the relevant JIRA ticket number when applicable. When multiple JIRA tickets are related, they may be suffixed to the title or mentioned within the commit message instead.

Examples include:

```
CXX-XXXX Resolve an issue
```

```
Fix several related issues (CXX-AAAA, CXX-BBBB)

* Commit A description
* Commit B description
* Commit C description
```

```
CXX-XXXX Implement a feature resolving several related issues

* CXX-AAAA Commit A description
* CXX-BBBB Commit B description
* Additional commit description
```

Refer to Chris Beams' guidelines for
[How to Write a Git Commit Message](http://chris.beams.io/posts/git-commit/).

## Formatting

Format files with [ClangFormat](https://clang.llvm.org/docs/ClangFormat.html) by running:

```bash
uv run --frozen etc/format.py
```
