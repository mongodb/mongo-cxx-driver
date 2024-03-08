# Releasing the mongocxx driver

If doing a release on a version prior to 3.5.0, follow the old instructions from
the shared Google drive. Go to File -> Version History -> See Version History
and select the version "Pre CXX-584".

## Ensure tests are passing

Ensure the latest commit has run tests on the Evergreen waterfall.

For a minor release this should be the
[waterfall](https://spruce.mongodb.com/commits/mongo-cxx-driver) tracking the
master branch (requires auth).
For a patch release this is the waterfall tracking the latest release. E.g. if you are
releasing 1.2.3, then refer to the the waterfall tracking
[releases/v1.2](https://spruce.mongodb.com/commits/mongo-cxx-driver-latest-release)
(requires auth).

If there are test failures, ensure they are at least expected or not introduced
by changes in the new release.

## Check fixVersions in Jira

Ensure that all tickets under the
[version to be released](https://jira.mongodb.com/projects/CXX?selectedItem=com.atlassian.jira.jira-projects-plugin%3Arelease-page&status=unreleased)
are in `Closed` status on the C++ Driver releases page. If not, bulk change open
tickets that will NOT be in the release to a new version (create it if
necessary).

For a patch release, check that all tickets for the version to be released have
changes cherry-picked onto the release branch. This is indicated by a comment on
the ticket. Here is an
[example comment](https://jira.mongodb.com/browse/CXX-2650?focusedCommentId=5271981&page=com.atlassian.jira.plugin.system.issuetabpanels:comment-tabpanel#comment-5271981).

## Audit Jira ticket titles and types

Update Jira ticket types and titles as appropriate.
User-facing issues should generally be either "Bug" or "New Feature".
Non-user facing issues should generally be "Task" tickets.

## Update CHANGELOG.md

Check Jira for tickets closed in this fix version. Consider updating CHANGELOG.md
with notable changes not already mentioned.

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

If doing a minor release (e.g. releasing r1.2.0, with a zero patch component),
stay on the master branch. You will create a new `releases/vX.Y` branch later in
the instructions. If doing a patch release (e.g. releasing rX.Y.Z with non-zero
`Z`), check out the corresponding release branch, which should be an existing
`releases/vX.Y` branch.

Create a tag for the commit to serve as the release (or release candidate):

```
git tag r1.2.3
```

## Run make_release.py

`make_release.py` creates the distribution tarball
(e.g. mongo-cxx-driver-r1.2.3.tar.gz), interacts with Jira, and drafts the
release on GitHub.

To see all available options, run with `--help`

```
python ./etc/make_release.py --help
```

It requires the following:

- A GitHub token. Go to the GitHub settings page
  [Personal Access Tokens](https://github.com/settings/tokens) and create a
  token.  Save the token secret to `~/.secrets/github_token.txt`.
- Jira OAuth credentials. Ask for these from a team member.
  Save it to `~/.secrets/jira_creds.txt`.

Run the release script with the git tag created above as an argument and
`--dry-run` to test for unexpected errors.

```
python ./etc/make_release.py \
    --dry-run \
    --jira-creds-file ~/.secrets/jira_creds.txt \
    --github-token-file ~/.secrets/github_token.txt \
    r1.2.3
```

If all goes well, run the command again without `--dry-run`, which should build
and test the tarball and draft the GitHub release.

### Troubleshooting make_release.py

If an error occurs, inspect logs the script produces, and troubleshoot as
follows:

- Use `--dry-run` to prevent unrecoverable effects.
- If building the C driver fails, use an existing C driver build (ensure it is
  the right version) with `--with-c-driver /path/to/cdriver/install`.
- Use `--skip-distcheck` to bypass time consuming checks when building the
  distribution tarball.
- If the script succeeded at creating the distribution tarball, pass it directly
  with `--dist-file ./build/mongo-cxx-driver-r1.2.3.tar.gz`.

## Push the tag

Review the build output and, assuming the distcheck target is successful, push
the tag into the main remote:

```
git push git@github.com:mongodb/mongo-cxx-driver.git refs/tags/r1.2.3
```

### Release the Version in GitHub

Review the generated release draft on GitHub, then publish the release:

```
Edit -> Publish Release
```

## Release the Version in Jira

Navigate to the
[fixVersions page on Jira](https://jira.mongodb.com/plugins/servlet/project-config/CXX/versions?status=unreleased).
Click the "..." next to the version you are about to release and select
"Release".

## Update releases/stable branch if needed

The `releases/stable` branch tracks the most recent "stable" release for users
who install from the git repository.

After any stable release (i.e. not an alpha, beta, RC, etc. release), check out
the `releases/stable` branch, reset it to the new release tag, and force push it
to the repo:

```
git checkout releases/stable
git reset --hard r1.2.3
git push -f origin releases/stable
```

## Generate and Publish Documentation

Documentation generation must be run after the release tag has been made and
pushed.

- Checkout the master branch.
- Edit `etc/apidocmenu.md` and add the released version in the `mongocxx` column
  following the established pattern. If this is a minor release (x.y.0), revise
  the entire document as needed.
- Edit `docs/content/_index.md` and `README.md` to match.
- Edit the `Installing the MongoDB C driver` section of
  `docs/content/mongocxx-v3/installation/advanced.md` to reflect libmongoc
  requirements.
- Edit `docs/content/mongocxx-v3/installation/linux.md`,
  `docs/content/mongocxx-v3/installation/macos.md` and
  `docs/content/mongocxx-v3/installation/windows.md`.
   If the release was not a release candidate, update `Step 2` to reflect the
   new latest stable version to download.
- Edit `etc/generate-all-apidocs.pl` and add the new release version to the
  `@DOC_TAGS` array, following the established pattern.
- Commit these changes:
  `git commit -am "Prepare to generate r1.2.3 release documentation"`
- Ensure you have `doxygen` and `hugo` installed and up to date.
- Run `git clean -dxf` to clear out all extraneous files.
- Configure with `cmake` in the `build` directory as you usually would.
- Build docs locally to test.
  - To test Hugo documentation, run the `docs` build target with
    `cmake --build ./build --target docs`.
  - To test generation of all API docs for all tags, build the  `doxygen-all`
    target with `cmake --build ./build --target doxygen-all` and be prepared to
    wait a while.
- To generate and deploy documentation to GitHub Pages, build both the
  `hugo-deploy` and `doxygen-deploy` targets. The doxygen build will take a long
  time.
  - `cmake --build ./build --target hugo-deploy`
  - `cmake --build ./build --target doxygen-deploy`
- If the release was not a release candidate, update symlinks
  - Check out the `gh-pages` branch and git pull the deployed docs.
  - Update the `api/mongocxx-v3` symlink to point to the newly released version.
    If a minor version bump has occurred, revise the symlink structure as
    needed. Make sure `current` always points to a symlink tracking the latest
    stable release branch.
  - Commit and push the symlink change:
    `git commit -am "Update symlink for r1.2.3"`
- Wait a few minutes and verify mongocxx.org has updated.
- Checkout the master branch. Push the commit containing changes to `etc/` and
  `docs/`. This may require pushing the commit to a fork of the C++ Driver
  repository and creating a pull request.

## Homebrew
This requires a macOS machine.
If this is a stable release, update the [mongo-cxx-driver](https://github.com/Homebrew/homebrew-core/blob/master/Formula/mongo-cxx-driver.rb) homebrew formula, using: `brew bump-formula-pr --url <tarball url>`

Example:
`brew bump-formula-pr mongo-cxx-driver --url https://github.com/mongodb/mongo-cxx-driver/releases/download/r3.7.3/mongo-cxx-driver-r3.7.3.tar.gz`

## vcpkg
Submit a PR or create an issue to update the vc-pkg file for mongo-c-driver.
To submit an issue, follow: https://github.com/microsoft/vcpkg/issues/new/choose. Example: https://github.com/microsoft/vcpkg/issues/34984

## Conan
Submit a PR or create an issue to update the Conan recipe for mongo-c-driver.
To submit an issue, follow: https://github.com/conan-io/conan-center-index/issues/new/choose/. Example: https://github.com/conan-io/conan-center-index/issues/21006

## Comment on the generated DOCSP ticket

Minor releases generate a DOCSP ticket. Add a comment to the generated DOCSP ticket describing if the
[MongoDB Compatibility Table](https://www.mongodb.com/docs/drivers/cxx/#mongodb-compatibility)
or [Language Compatibility Table](https://www.mongodb.com/docs/drivers/cxx/#language-compatibility)
should be updated. Generally, only a minor release will require updates.
(See [DOCSP-30876](https://jira.mongodb.com/browse/DOCSP-30876) for an example.)

## Announce on Community Forums

Announce Post to the [developer community forum](https://community.mongodb.com)
under `Product & Driver Announcements` with the tag `cxx`.

See this
[example announcement](https://www.mongodb.com/community/forums/t/mongodb-c-11-driver-3-9-0-released/252724)
of the stable release of 3.9.0.

## Branch if necessary

If doing a new minor release `x.y.0` (e.g. a `1.2.0` release), create branch
`releases/vx.y`  (e.g `releases/v3.8`).

Push the new branch:

```
git push --set-upstream origin releases/v3.8
```

The new branch should be continuously tested on Evergreen. Create a BUILD ticket
to request the build team create new Evergreen project to track the
`releases/vx.y` branch (see BUILD-5666 for an example).

## Docker Image Build and Publish

We maintain the docker images found in the
[mongodb/mongo-cxx-driver](https://hub.docker.com/r/mongodb/mongo-cxx-driver/)
Docker Hub repo.

To publish a new image here, you will need the password for the following
service account: `svcmongodbcxxdriverdockerbo219`. The team lead can share
this password with you as needed.

First, in `mongo-cxx-driver/extras/docker/generate.py` bump the following
version numbers as appropriate:
- MONGOCXX_VERSION
- MONGOC_VERSION
- MONGOCRYPT_VERSION

**Make sure you run `python3 generate.py` to generate the files for building the
images!**

Next, build the image. Prefer doing a no-cache-build with a clean docker cache.

> As of now, company policy only allows for redhat-ubi images, so only build and
> push the redhat-ubi based image.

```
$ yes | docker system prune -a
$ cd mongo-cxx-driver/extras/docker/redhat-ubi-9.3
$ make nocachebuild
```

Then, test that the image works as expected:
```
$ cd redhat-ubi-9.3/
$ make test
```

If the test passes, you will see the following output with the version number
of the driver that you are currently releasing:
```
mongo-cxx-driver version: 3.9.0
THE redhat-ubi-9.3 IMAGE WORKS!
```

If the test passes, then check in the bumped version numbers, and get it merged
into master.

Once the Evergreen docker tests pass, login to the service account:
```
$ docker login --username svcmongodbcxxdriverdockerbo219
Password: <INSERT PASSWORD HERE>
```

We now need to perform a multi-arch build. We currently support both AMD64
(x86_64) and ARM64 (aarch64). You will need to have `qemu` installed in order to
emulate non-native architectures in Docker. The following commands will setup a
buildx builder, build the images for both AMD64 and ARM64, then push those
images to Docker Hub. To say this again, the `--push` flag below means that the
images will be pushed to the public Docker Hub repository, so **only run these
commands when you are ready to push the images!**
```
$ docker buildx create --name mybuilder --use --bootstrap
$ docker buildx build --push --platform linux/amd64,linux/arm64 --tag mongodb/mongo-cxx-driver:<VERSION NUMBER>-redhat-ubi-9.3 --tag mongodb/mongo-cxx-driver:latest .
```

Update the `Tags` section of the Docker Hub
[Overview](https://hub.docker.com/r/mongodb/mongo-cxx-driver) to list the latest
tag and link it to the corresponding `Dockerfile` in Github. This requires privileged access to DockerHub. Ask other team members to update if you do not have access.

Update the `FROM` lines in each Dockerfile example such that they would pull
from the latest Docker Hub image release.

Check in the updated overview to this repo as the file ./extras/docker/README.md

## Handle Linux Distribution Packages

Note: updates to these instructions should be synced to the corresponding C
driver process documentation Google doc.

### Debian

#### Build

- Checkout the appropriate release branch.
- For the first Debian package release on a new release branch, edit
  `debian/gbp.conf` and update the `upstream-branch` and `debian-branch`
  variables to match the name of the new release branch (e.g., `releases/v3.x`);
  both variables will have the same value
- The Debian package release is made after the upstream release has been tagged
- Create a new changelog entry (use the command `dch -i` to ensure proper
  formatting), then adjust the version number on the top line of the changelog
  as appropriate

```
DEBEMAIL='my-email@mongodb.com' DEBFULLNAME='FIRSTNAME LASTNAME' dch -v VERSION
```

- Make any other necessary changes to the Debian packaging components
  (e.g., update to standards version, dependencies, descriptions, etc.) and make
  relevant entries in `debian/changelog` as needed
- If this release fixes any Debian bugs that are tracked in the Debian bug
  tracking system (links below in [Post Build](#post-build)), then note that
  the bug is closed with this release in `debian/changelog`, for example:
```
  * New upstream release (Closes: #1042682)
```

- Use `git add` to stage the changed files for commit (only files in the
  `debian/` directory should be committed), then commit them (the `debcommit`
  utility is helpful here). A common commit message for this stage is:

```
(Debian packaging) New upstream release
```

- Create a chroot environment using cowbuilder.

```
$ sudo cowbuilder --create --mirror http://ftp.us.debian.org/debian/ --distribution sid --basepath /var/cache/pbuilder/base-sid.cow
```

- If you already have a chroot environment setup, then update it.

```
$ sudo cowbuilder --update --mirror http://ftp.us.debian.org/debian/ --distribution sid --basepath /var/cache/pbuilder/base-sid.cow
```

- Create the file `~/.gbp.conf` with the following text.

```
[DEFAULT]
cleaner = true
pbuilder = True
pbuilder-options = --source-only-changes

[buildpackage]
#sign-tags = True
export-dir = ../build-area/
```

- Build the package with `gbp buildpackage`

```
DH_VERBOSE=1 DEB_BUILD_OPTIONS="parallel=$(nproc)" gbp buildpackage --git-dist=sid
```

- Inspect the resulting package files (at a minimum use `debc` on the `.changes`
  file in order to confirm files are installed to the proper locations by the
  proper packages and also use `lintian` on the `.changes` file in order to
  confirm that there are no unexpected errors or warnings; the `lintian` used
  for this check should always be the latest version as it is found in the
  unstable distribution). This is easiest done using the Sid chroot that was
  created in the previous steps.

```
$ sudo cowbuilder --login --basepath /var/cache/pbuilder/base-sid.cow/ --bindmounts $HOME
# apt update && apt install -y lintian
# lintian -viI mongo-cxx-driver_3.7.2-1_amd64.changes
```

- You may need to update to the latest Debian policy, which you can do the
  following to see the latest policy.

```
# apt install debian-policy
# zless /usr/share/doc/debian-policy/upgrading-checklist.txt.gz
```

- If any changes are needed, make them, commit them, and rebuild the package
- Edit changelog, change UNRELEASED to experimental.

```
$ DEBEMAIL='my-email@mongodb.com' DEBFULLNAME='FIRSTNAME LASTNAME' dch -r -D experimental
```

- Commit this change with the following message:

```
(Debian packaging) ready for release
```

- It may be desirable to squash multiple commits down to a single commit before
  building the final packages
- After you finish making commits, build the Debian package for one final time,
  and if you are not a Debian maintainer, then give the packages to a debian
  maintainer to do the two steps below:
  - Once the final packages are built, they can be signed and uploaded and the
    version can be tagged using the `--git-tag` option of `gbp buildpackage`
  - Sign and upload the package, push the commits on the release branch and
    the master branch to the remote, and push the Debian package tag.
- After the commit has been tagged, switch from the release branch to the
  `master` branch and cherry-pick the commit(s) made on the release branch that
  touch only the Debian packaging (this will ensure that the packaging and
  especially the changelog on the master remain up to date)
- Open a PR with the cherry-picks and use the `Rebase and merge` merge strategy.

#### Post Build

After the package has been accepted into Debian, make sure to check if there are
any bugs or build failures. Debian tests builds on more architectures than we
officially support and test on in Evergreen, so there are occasionally
surprising build failures on exotic architectures.

Bug tracking can be found here for the following projects:
- [libmongocrypt](https://bugs.debian.org/cgi-bin/pkgreport.cgi?src=libmongocrypt)
- [mongo-c-driver](https://bugs.debian.org/cgi-bin/pkgreport.cgi?src=mongo-c-driver)
- [mongo-cxx-driver](https://bugs.debian.org/cgi-bin/pkgreport.cgi?src=mongo-cxx-driver)

Build logs can be found here:
- [libmongocrypt](https://buildd.debian.org/status/package.php?p=libmongocrypt)
- [mongo-c-driver](https://buildd.debian.org/status/package.php?p=mongo-c-driver)
- [mongo-cxx-driver](https://buildd.debian.org/status/package.php?p=mongo-cxx-driver)

Tracking the status of the following projects can be found here:
- [libmongocrypt](https://tracker.debian.org/pkg/libmongocrypt)
- [mongo-c-driver](https://tracker.debian.org/pkg/mongo-c-driver)
- [mongo-cxx-driver](https://tracker.debian.org/pkg/mongo-cxx-driver)

### Fedora

- After CXX-2226, the RPM spec file has been vendored into the project; it needs
  to be updated periodically
- Starting in the project root directory, retrieve the latest spec file:

```
curl -L -o .evergreen/mongo-cxx-driver.spec https://src.fedoraproject.org/rpms/mongo-cxx-driver/raw/rawhide/f/mongo-cxx-driver.spec
```

- Confirm that our spec patch applies to the new downstream spec:

```
patch --dry-run -d .evergreen -p0 -i spec.patch
```

- If the patch command fails, rebase the patch
- For a new minor release (e.g., 3.6.0, 3.7.0, etc.), then ensure that the patch
  updates the `up_version` to be the NEXT minor version (e.g., when releasing
  1.2.0, the spec patch should update `up_version` to 1.3.0); this is necessary
  to ensure that the spec file matches the tarball created by the dist target;
  if this is wrong, then the `rpm-package-build` task will fail in the next step
- Additionally, ensure that any changes made on the release branch vis-a-vis the
  spec file are also replicated on the `master` or `main` branch
- Test the build with something like this:

```
evergreen patch -p mongo-cxx-driver -v packaging -t rpm-package-build -f
```

- There is no package upload step, since the downstream maintainer handles that
  and we only have the Evergreen task to ensure that we do not break the package
  build
- The same steps need to be executed on active release branches
  (e.g., `releases/v3.6`), which can usually be accomplished via
  `git cherry-pick` and then resolving any minor conflicts
