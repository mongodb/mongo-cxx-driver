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

Format files with [ClangFormat](https://clang.llvm.org/docs/ClangFormat.html) using the [etc/clang_format.py](https://github.com/mongodb/mongo-cxx-driver/blob/master/etc/clang_format.py) script.

The script must be run in the project root directory to ensure the [.clang-format](https://github.com/mongodb/mongo-cxx-driver/blob/master/.clang-format) configuration file is used properly.

```bash
# Allow the script to download the correct ClangFormat release version.
python2 ./etc/clang-format.py format

# Provide a path to an existing ClangFormat binary to use instead.
MONGO_CLANG_FORMAT="path/to/clang-format" python2 ./etc/clang-format.py format
```

> [!NOTE]
> ClangFormat results may differ across release versions. When using a preinstalled ClangFormat binary, its version must be consistent with the `CLANG_FORMAT_VERSION` variable defined in the `etc/clang_format.py` script.
