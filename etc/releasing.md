# Releasing the mongocxx driver

If doing a release on a version prior to 3.5.0, follow the old instructions from the shared Google drive. Go to File -> Version History -> See Version History and select the version "Pre CXX-584".

## Ensure tests are passing
Ensure the latest commit has run tests on the Evergreen waterfall.

For a minor release this should be the waterfall tracking the master branch:
https://evergreen.mongodb.com/waterfall/cxx-driver
For a patch release this is the waterfall tracking that branch. E.g. if you are releasing 3.6.4, then the waterfall tracking releases/v3.6:
https://evergreen.mongodb.com/waterfall/cxx-driver-v3.6

If there are test failures, ensure they are at least expected or not introduced by changes in the new release.

In particular, check that the "Ubuntu 18.04 with minimum libmongoc" variant is passing to ensure that backports have not introduced a dependency on later versions of libmongoc.

## Check fixVersions in Jira

Ensure that all tickets under the [version to be released](https://jira.mongodb.org/projects/CXX?selectedItem=com.atlassian.jira.jira-projects-plugin%3Arelease-page&status=unreleased) are in `Closed` status on the C++ Driver releases page. If not, bulk change open tickets that will NOT be in the release to a new version (create it if necessary).

## Audit Jira ticket titles and types for use in release notes
From the releases page click the "Release Notes" link to see a summary of tickets to be included in release notes. Update the ticket type and title as appropriate. User-facing issues should generally be either "Bug" or "New Feature". Non-user facing issues should generally be "Task" tickets (and will be omitted later, so you can ignore them here).

## Clone and set up environment
Do a fresh clone, to avoid local git branches or IDE files from interfering.
```
git clone git@github.com:mongodb/mongo-cxx-driver.git mongo-cxx-driver-release
cd mongo-cxx-driver-release
```

Start a Python 3 virtual environment and install required packages with pip.
```
python3 -m venv ~/virtualenv
. ~/virtualenv/bin/activate
pip install -r etc/requirements.txt
```

## Tag the release

If doing a minor release (e.g. releasing r3.6.0), stay on the master branch. You will create a new `releases/v3.6` branch later in the instructions.
If doing a patch release (e.g. releasing r3.6.1), check out the corresponding release branch, which should be `releases/v3.6`.

Create a tag for the commit to serve as the release (or release candidate):

```
git tag r3.6.0
```

## Run make_release.py

`make_release.py` creates the distribution tarball (e.g. mongo-cxx-driver-r3.6.0.tar.gz), interacts with Jira, and drafts the release on GitHub.

To see all available options, run with `--help`
```
python ./etc/make_release.py --help
```

It requires the following:
- A GitHub token. Go to the GitHub settings page [Personal Access Tokens](https://github.com/settings/tokens) and create a token. Save the token secret to `mongo-cxx-driver-release/github_token.txt`.
- Jira OAuth credentials. Ask for these from a team member. Save it to `mongo-cxx-driver-release/jira_creds.txt`.

Run the release script with the git tag created above as an argument.
```
python ./etc/make_release.py r3.6.0
```

If all goes well, this should build and test the tarball and draft the GitHub release.

### Troubleshooting make_release.py
If an error occurs, inspect logs the script produces, and troubleshoot as follows:
- Use `--dry-run` to prevent unrecoverable effects.
- If building the C driver fails, use an existing C driver build (ensure it is the right version) with `--with-c-driver /path/to/cdriver/install`.
- Use `--skip-distcheck` to bypass time consuming checks when building the distribution tarball.
- If the script succeeded at creating the distribution tarball, pass it directly with `--dist-file ./build/mongo-cxx-driver-r3.6.0.tar.gz`.

## Push the tag
Review the build output and, assuming the distcheck target is successful, push the tag

```
git push origin r3.6.0
```

## Release the Version in Jira
Navigate to the [fixVersions page on Jira](https://jira.mongodb.org/plugins/servlet/project-config/CXX/versions?status=unreleased). Click the "..." next to the version you are about to release and select "Release".

## Update releases/stable branch if needed
The `releases/stable` branch tracks the most recent "stable" release for users who install from the git repository.

After any stable release (i.e. not an alpha, beta, RC, etc. release), check out the `releases/stable` branch, reset it to the new release tag, and force push it to the repo:

```
git checkout releases/stable
git reset --hard r3.6.0
git push -f origin releases/stable
```

## Generate and Publish Documentation

Documentation generation must be run after the release tag has been made and pushed.

- Checkout the master branch.
- Edit `etc/apidocmenu.md` and add the released version in the `mongocxx` column following the established pattern. If this is a major release (x.y.0), revise the entire document as needed.
- Edit `docs/content/index.md` and `README.md` to match.
- Edit `etc/generate-all-apidocs.pl` and add the new release version to the `@DOC_TAGS` array, following the established pattern.
- Edit `docs/content/mongocxx-v3/installation/linux.md`, `docs/content/mongocxx-v3/installation/macos.md` and `docs/content/mongocxx-v3/installation/windows.md` and update `Step 1` to reflect to libmongoc requirements. If the release was not a release candidate, update `Step 3` to reflect the new latest stable version to download.
- Commit these changes `git commit -am "Prepare to generate r3.6.0 release documentation"`
- Ensure you have doxygen and hugo installed and up to date.
- Run `git clean -dxf` to clear out all extraneous files.
- Configure with `cmake` in the `build` directory as you usually would.
- Build docs locally to test.
    - To test Hugo documentation, run the `docs` build target with `cmake --build ./build --target docs`.
    - To test generation of all API docs for all tags, build the  `doxygen-all` target with `cmake --build ./build --target doxygen-all` and be prepared to wait a while.
- To generate and deploy documentation to GitHub Pages, build both the `hugo-deploy` and `doxygen-deploy` targets. The doxygen build will take a long time.
    - `cmake --build ./build --target hugo-deploy`
    - `cmake --build ./build --target doxygen-deploy`
- If the release was not a release candidate, update symlinks
    - Check out the `gh-pages` branch and git pull the deployed docs.
    - Update the `api/mongocxx-v3` symlink to point to the newly released version. If a major version bump has occurred, revise the symlink structure as needed. Make sure `current` always points to a symlink tracking the latest stable release branch.
    - Commit and push the symlink change: `git commit -am "Update symlink for r3.6.0"`
- Wait a few minutes and verify mongocxx.org has updated.
- Push the updated documentation with `git push origin master`.

## File a DOCSP ticket if needed
If the MongoDB manual [driver-server compatibility matrix or language compatibility matrix](https://docs.mongodb.com/drivers/driver-compatibility-reference) should be updated, file a DOCSP ticket. This generally will only apply to a minor release. (See DOCSP-3504 for an example).

## Announce on Community Forums
Announce
Post to https://community.mongodb.com under `Product & Driver Announcements` with the tag `cxx`.

Here is an example announcement of the stable release of 3.5.0:
https://developer.mongodb.com/community/forums/t/mongodb-c-11-driver-3-5-0-released/2182

Here is an example announcement of a release candidate of 3.6.0:
https://developer.mongodb.com/community/forums/t/mongodb-c-11-driver-3-6-0-rc0-released/6960

## Branch if necessary
If doing a new minor release (e.g. a `x.y.0` release), create branch `releases/vx.y`  (e.g `releases/v3.6`).

Push the new branch:
```
git push --set-upstream origin releases/v3.6
```

The new branch should be continuously tested on Evergreen. Create a BUILD ticket to request the build team create new Evergreen project to track the `releases/vx.y` branch (see BUILD-5666 for an example).
