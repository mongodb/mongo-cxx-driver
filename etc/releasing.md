# Releasing the mongocxx driver

The release steps for a patch release differ slightly from release steps for a minor or major release.
Ensure steps specific to patch releases are correctly applied or skipped depending on the type of release.

`X.Y.Z` is used to refer to a release version with major version `X`, minor version `Y`, and patch version `Z`. For sake of examples, `1.2.3` is used as the new patch release version and `1.3.0` is used as the non-patch release version (implying the current stable release version is `1.2.2`).

> [!WARNING]
> Do NOT accidentally use example version numbers when executing release commands and scripts!

## Remote Repositories

In some steps, the main remote repository must be distinguished from a personal fork remote repository.

The main repository is referred to as `upstream`.

The fork repository is referred to as `origin`.

The main repository may be explicitly named during initial clone via `--origin <name>`:

```bash
git clone -o upstream git@github.com:mongodb/mongo-cxx-driver.git
```

The fork repository may be subsequently added via `git remote add`:

```bash
git remote add origin git@github.com:<username>/mongo-cxx-driver.git
```

An existing remote may be renamed using `git remote rename <old> <new>`.

## Secrets and Credentials

> [!WARNING]
> Avoid directly typing secret values as command-line arguments. Save the secret values to the relevant file using an editor. Use `source path/to/secret.txt && program --argument "${SECRET:?}"` instead of `program --argument "<secret>"`.

Some release steps require one or more of the following secrets.

- A GitHub Personal Access Token (Classic).
  - Location: `~/.secrets/github-token.txt`
  - Format:
    ```
    <github_token>
    ```
  - Instructions: go to the GitHub settings page
  [Personal Access Tokens](https://github.com/settings/tokens) and create a
  (classic) token with the "repo" scope selected.

    Configure SSO to authorize this token for the `mongodb` organization. (Do not forget this step!)
- Jira OAuth credentials.
  - Location: `~/.secrets/jira-creds.txt`
  - Format:
    ```
    Username: evergreen.jirareleases@mongodb.com

    access_token : <access_token>
    access_token_secret : <access_token_secret>
    consumer_key : <consumer_key>
    key_cert: -----BEGIN RSA PRIVATE KEY-----\n...\n-----END RSA PRIVATE KEY-----
    ```
    Note the `"\n"` strings must be preserved as-is in the key_cert value.
- Artifactory credentials.
  - Location: `~/.secrets/artifactory-creds.txt`:
  - Format:
    ```bash
    ARTIFACTORY_USER=<username>
    ARTIFACTORY_PASSWORD=<password>
    ```
- Garasign credentials
  - Location: `~/.secrets/garasign-creds.txt`
  - Format:
    ```bash
    GRS_CONFIG_USER1_USERNAME=<username>
    GRS_CONFIG_USER1_PASSWORD=<password>
    ```
- Silk credentials.
  - Location: `~/.secrets/silk-creds.txt`
  - Format:
    ```bash
    SILK_CLIENT_ID=<client_id>
    SILK_CLIENT_SECRET=<client_secret>
    ```
- Snyk credentials.
  - Location: `~/.secrets/snyk-creds.txt`
  - Format:
    ```bash
    SNYK_API_TOKEN=<token>
    ```

## Pre-Release Steps

> [!TIP]
> Pre-release steps should preferably be done regularly and PRIOR to the scheduled release date.

### Evergreen

Ensure Evergreen has run mainline tasks on the latest commit.

For non-patch releases, check the [mongo-cxx-driver](https://spruce.mongodb.com/commits/mongo-cxx-driver) project mainline.

For patch releases, check the [mongo-cxx-driver-latest-release](https://spruce.mongodb.com/commits/mongo-cxx-driver-latest-release) project

Ensure there are no new or unexpected task failures.

### Coverity

Ensure there are no new or unexpected issues with High severity or greater.

Update the [SSDLC Report spreadsheet](https://docs.google.com/spreadsheets/d/1sp0bLjj29xO9T8BwDIxUk5IPJ493QkBVCJKIgptxEPc/edit?usp=sharing) with any updates to new or known issues.

### SBOM Lite

Ensure the list of bundled dependencies in `etc/purls.txt` is up-to-date. If not, update `etc/purls.txt`.

If `etc/purls.txt` was updated, update the SBOM Lite document using the following command(s):

```bash
# Artifactory and Silk credentials.
. $HOME/.secrets/artifactory-creds.txt
. $HOME/.secrets/silk-creds.txt

# Output: "Login succeeded!"
podman login --password-stdin --username "${ARTIFACTORY_USER:?}" artifactory.corp.mongodb.com <<<"${ARTIFACTORY_PASSWORD:?}"

# Ensure latest version of SilkBomb is being used.
podman pull artifactory.corp.mongodb.com/release-tools-container-registry-public-local/silkbomb:1.0

# Output: "... writing sbom to file"
podman run \
  --env-file "$HOME/.secrets/silk-creds.txt" \
  -it --rm -v "$(pwd):/pwd" \
  artifactory.corp.mongodb.com/release-tools-container-registry-public-local/silkbomb:1.0 \
  update -p "/pwd/etc/purls.txt" -i "/pwd/etc/cyclonedx.sbom.json" -o "/pwd/etc/cyclonedx.sbom.json"
```

Commit the latest version of the SBOM Lite document into the repo as `etc/cyclonedx.sbom.json`. (This may just be a modification of the timestamp.)

### Augmented SBOM

Ensure the `silk-check-augmented-sbom` task is passing on Evergreen for the relevant release branch. If it is passing, nothing needs to be done.

 If the `silk-check-augmented-sbom` task was failing, update the Augmented SBOM document using the following command(s):

```bash
# Artifactory and Silk credentials.
. $HOME/.secrets/artifactory-creds.txt
. $HOME/.secrets/silk-creds.txt

# Output: "Login succeeded!"
podman login --password-stdin --username "${ARTIFACTORY_USER:?}" artifactory.corp.mongodb.com <<<"${ARTIFACTORY_PASSWORD:?}"

# Ensure latest version of SilkBomb is being used.
podman pull artifactory.corp.mongodb.com/release-tools-container-registry-public-local/silkbomb:1.0

# Output: "... writing sbom to file"
podman run \
  --env-file "$HOME/.secrets/silk-creds.txt" \
  -it --rm -v "$(pwd):/pwd" \
  artifactory.corp.mongodb.com/release-tools-container-registry-public-local/silkbomb:1.0 \
  download --silk-asset-group "mongo-cxx-driver" -o "/pwd/etc/augmented.sbom.json"
```

Review the contents of the new Augmented SBOM and ensure any new or known vulnerabilities with severity "Medium" or greater have a corresponding JIRA ticket (CXX or VULN) that is scheduled to be resolved within its remediation timeline.

Update the [SSDLC Report spreadsheet](https://docs.google.com/spreadsheets/d/1sp0bLjj29xO9T8BwDIxUk5IPJ493QkBVCJKIgptxEPc/edit?usp=sharing) with any updates to new or known vulnerabilities.

Update `etc/third_party_vulnerabilities.md` with any updates to new or known vulnerabilities for third party dependencies that have not yet been fixed by the upcoming release.

Commit the latest version of the Augmented SBOM document into the repo as `etc/augmented.sbom.json`. The Augmented SBOM document does not need to be updated if the `silk-check-augmented-sbom` was not failing (in which case the only changes present would a version bump or timestamp update).

### Check Snyk

Inspect the list of projects in the latest report for the `mongodb/mongo-cxx-driver` target in [Snyk](https://app.snyk.io/org/dev-prod/).

Deactivate any projects that will not be relevant in the upcoming release. Remove any projects that are not relevant to the current release.

### Check Jira

Inspect the list of tickets assigned to the version to be released on [Jira](https://jira.mongodb.com/projects/CXX?selectedItem=com.atlassian.jira.jira-projects-plugin%3Arelease-page&status=unreleased).

Ensure all related ticket statuses are `Closed` (with the exception of the ticket tracking the release itself).

For tickets that will not be part of this release, update their fix version accordingly.

> [!IMPORTANT]
> For a patch release, ensure the commits for all related tickets have been cherry-picked onto the release branch.

> [!TIP]
> This is a good time to also update entries in `CHANGELOG.md` corresponding to the tickets whose fix versions are being updated.

Update the contents of related Jira tickets as appropriate (improve the title, clarify the description, link related tickets, etc.).

> [!NOTE]
> A ticket whose changes may impact users should either be a "Bug" or "New Feature".
> Otherwise, the ticket should typically be a "Task".

## Release Steps

### Update CHANGELOG...

This step depends on the release type.

#### ... for a Patch Release

Checkout the current release branch `releases/vX.Y` (e.g. for a patch release `1.2.3`, the current release branch is `releases/v1.2`).

```bash
git fetch upstream
git checkout releases/vX.Y
```

Update `CHANGELOG.md` with a summary of important changes in this release. Consult the list of related Jira tickets (updated ealier) as well as the list of commits since the last release.

Remove the `[Unreleased]` tag from the relevant patch release section, e.g. for release `1.2.3`:

```md
## 1.2.3

...

## 1.2.2

...

```

Commit and push the updates to `CHANGELOG.md` to `releases/vX.Y` (a PR is not required):

```bash
git commit -m 'Update CHANGELOG for X.Y.Z'
git push upstream releases/vX.Y
```

#### ... for a Non-Patch Release

Create a new branch named `pre-release-changes` on `master`. This branch will be used to later create a PR prior to release.

```bash
git fetch upstream
git checkout -b pre-release-changes upstream/master
```

Update `CHANGELOG.md` with a summary of important changes in this release. Consult the list of related Jira tickets (updated earlier) as well as the list of commits since the last release.

Remove the `[Unreleased]` tag from the relevant non-patch release section, e.g. for release `1.3.0`:

```md
## 1.3.0

...

## 1.2.2

...

```

> [!IMPORTANT]
> If there are entries under an unreleased patch release section with the old minor release number, move the entries into this release's section and remove the unreleased patch release section. For example, for a `1.3.0` minor release, move entries from `1.2.3 [Unreleased]` to `1.3.0` and remove `1.2.3 [Unreleased]`. Due to cherry-picking, a non-patch release should always (already) contain the changes targeting a patch release with a prior minor version number. (This is analogous to updating the fix version of Jira tickets, as done earlier.)

Commit the updates to `CHANGELOG.md`.

```bash
git commit -m 'Update CHANGELOG for X.Y.Z'
```

Push the `pre-release-changes` branch to a fork repository and create a PR to merge `pre-release-changes` onto `master`:

```bash
git remote add origin git@github.com:<username>/mongo-cxx-driver.git
git push -u origin pre-release-changes
```

Once the PR is merged, delete the `pre-release-changes` branch.

### Fresh Clone

To avoid potential complications during the release process, clone the updated repository in a new directory:

```bash
git clone -o upstream git@github.com:mongodb/mongo-cxx-driver.git mongo-cxx-driver-release
cd mongo-cxx-driver-release
```

Create and activate a fresh Python 3 virtual environment with required packages installed:

```bash

python3 -m venv ~/mongo-cxx-driver-release-venv # Outside the mongo-cxx-driver-release directory!
source ~/mongo-cxx-driver-release-venv/bin/activate
pip install -r etc/requirements.txt
```

### Create a Release Tag...

> [!IMPORTANT]
> Do NOT push the release tag immediately after its creation!

#### ... for a Patch Release

Checkout the release branch (containing the changes from earlier steps) and create a tag for the release.

```bash
git checkout releases/vX.Y
git tag rX.Y.Z
```

#### ... for a Non-Patch Release

Checkout the `master` branch (containing the changes from earlier steps) and create a tag for the release:

```bash
git checkout master
git tag rX.Y.0
```

> [!NOTE]
> A new release branch `releases/vX.Y` will be created later as part of post-release steps.

### Run etc/make_release.py

This script performs the following steps:

- create the distribution tarball (e.g. `mongo-cxx-driver-r1.2.3.tar.gz`),
- creates a signature file for the distribution tarball (e.g. `mongo-cxx-driver-r1.2.3.tar.gz.asc`),
- query Jira for release and ticket statuses, and
- creates a release draft on GitHub.

To see all available options, run with `--help`.

```
python ./etc/make_release.py --help
```

The following secrets are required by this script:

- GitHub Personal Access Token.
- Jira OAuth credentials.
- Artifactory credentials.
- Garasign credentials.

Run the release script with the git tag created above as an argument and
`--dry-run` to test for unexpected errors.

```bash
make_release_args=(
    --jira-creds-file ~/.secrets/jira-creds.txt
    --github-token-file ~/.secrets/github-token.txt
)
python ./etc/make_release.py "${make_release_args[@]:?}" --dry-run rX.Y.Z
```

> [!TIP]
> Export environment variables (e.g.`CMAKE_BUILD_PARALLEL_LEVEL`) to improve the speed of this command.

If an error occurs, inspect logs the script produces, and troubleshoot as
follows:

- Use `--dry-run` to prevent unrecoverable effects.
- If building the C driver fails, use an existing C driver build (ensure it is
  the right version) with `--with-c-driver /path/to/c-driver/install`.
- Use `--skip-distcheck` to bypass time consuming checks when building the
  distribution tarball.
- If the script succeeded at creating the distribution tarball, pass it directly
  with `--dist-file ./build/mongo-cxx-driver-rX.Y.Z.tar.gz`.

If all goes well, run the command again without `--dry-run`. This should update Jira and create a draft release on GitHub.

Verify the successful creation of the release draft on GitHub.

### Push the Release Tag

Push the release tag (created earlier) to the remote repository:

```bash
git push origin rX.Y.Z
```

### Release on GitHub

Verify the pushed release tag is detected by the release draft (refresh the page if necessary).

Review the contents of the release draft, then publish the release.

### Release on Jira

Navigate to the
[fixVersions page on Jira](https://jira.mongodb.com/plugins/servlet/project-config/CXX/versions?status=unreleased).

Click the "..." next to the relevant version and select "Release".

### Update releases/stable

The `releases/stable` branch tracks the most recent "stable" release for users
who install from the git repository.

> [!WARNING]
> This step does NOT apply to alpha, beta, release candidate (RC), or similar types of "unstable" release versions which may contain a suffix in its release tag.

Check out the `releases/stable` branch, hard-reset it to the new release tag, then force-push it
to the remote repository:

```bash
git checkout releases/stable
git reset --hard rX.Y.Z
git push -f upstream releases/stable
```

### Upload SSDLC Reports

Navigate to the [C++ Driver SSDLC Reports](https://drive.google.com/drive/folders/1q9RI55trFzHlh8McALSIAbT6ugyn8zlO) folder and update the master spreadsheet.

Once complete, make two copies of the spreadsheet.

Rename one copy to: "SSDLC Report: mongo-cxx-driver X.Y.Z". Leave this copy in this folder.

Rename the other copy to: "static_analysis_report-X.Y.Z". Move this copy into the [SSDLC Compliance Files](https://drive.google.com/drive/folders/1_qwTwYyqPL7VjrZOiuyiDYi1y2NYiClS) folder and name it.

Upload a copy of the `etc/ssdlc_compliance_report.md`, `etc/third_party_vulnerabilities.md`, and `etc/augmented.sbom.json` files. Rename the files with the version number `-X.Y.Z` suffix in their filenames as already done for other files in this folder.

> [!WARNING]
> Uploading a file into the SSDLC Compliance Files folder is an irreversible action! However, the files may still be renamed. If necessary, rename any accidentally uploaded files to "(Delete Me)" or similar.

Four new files should be present in the [SSDLC Compliance Files](https://drive.google.com/drive/folders/1_qwTwYyqPL7VjrZOiuyiDYi1y2NYiClS) folder following a release `X.Y.Z`:

```
ssdlc_compliance_report-X.Y.Z.md
third_party_vulnerabilities-X.Y.Z.md
static_analysis-X.Y.Z
augmented.sbom-X.Y.Z.json
```

## Post-Release Steps

### Create a New Release Branch

> [!IMPORTANT]
> The creation of a new release branch only applies to non-patch releases! Patch releases should continue to use the existing release branch.

For a new non-patch release `X.Y.0`, create a new branch `releases/vX.Y`:

```bash
git fetch upstream
git checkout -b releases/vX.Y upstream/master
```

Push the new branch to the remote repository:

```
git push origin releases/vX.Y
```

The new branch should be continuously tested on Evergreen. Update the "Display Name" and "Branch Name" of the [mongo-cxx-driver-latest-release Evergreen project](https://spruce.mongodb.com/project/mongo-cxx-driver-latest-release/settings/general) to refer to the new release branch.

The new branch should be tracked by Silk. Use the [create-silk-asset-group.py script](https://github.com/mongodb/mongo-c-driver/blob/master/tools/create-silk-asset-group.py) in the C Driver to create a new Silk asset group:

```bash
# Snyk credentials. Ask for these from a team member.
. ~/.secrets/silk-creds.txt.

# Ensure correct release version number!
version="X.Y"

create_args=(
  --silk-client-id "${SILK_CLIENT_ID:?}"
  --silk-client-secret "${SILK_CLIENT_SECRET:?}"
  --asset-id "mongo-cxx-driver-${version:?}" # Avoid '/' in Asset ID field.
  --project "mongo-cxx-driver-${version:?}"
  --branch "releases/v${version:?}"
  --code-repo-url "https://github.com/mongodb/mongo-cxx-driver"
  --sbom-lite-path="etc/cyclonedx.sbom.json"
)

python path/to/tools/create-silk-asset-group.py "${create_args[@]:?}"
```

### Update Snyk

> [!IMPORTANT]
> Run the Snyk commands in a fresh clone of the post-release repository to avoid existing build and release artifacts from affecting Snyk.

Checkout the new release tag.

Configure and build the CXX Driver with `BSONCXX_POLY_USE_MNMLSTC=ON` (force download of mnmlstc/core sources) and no `CMAKE_PREFIX_PATH` entry to an existing C Driver installation (force download of C Driver sources), then run:

```bash
# Snyk credentials. Ask for these from a team member.
. ~/.secrets/snyk-creds.txt

# The new release tag. Ensure this is correct!
release_tag="rX.Y.Z"

# Authenticate with Snyk dev-prod organization.
snyk auth "${SNYK_API_TOKEN:?}"

# Verify third party dependency sources listed in etc/purls.txt are detected by Snyk.
# If not, see: https://support.snyk.io/hc/en-us/requests/new
snyk_args=(
  --org=dev-prod
  --remote-repo-url=https://github.com/mongodb/mongo-cxx-driver/
  --target-reference="${release_tag:?}"
  --unmanaged
  --all-projects
  --detection-depth=10 # build/src/bsoncxx/third_party/_deps/core-install/include/core
  --exclude=extras # CXX-3042
)
snyk test "${snyk_args[@]:?}" --print-deps

# Create a new Snyk target reference for the new release tag.
snyk monitor "${snyk_args[@]:?}"
```

### Create Documentation Tickets

Create and checkout a new branch `post-release-changes` relative to `master` to contain documentation updates following the new release:

```bash
git fetch upstream
git checkout -b post-release-changes upstream/master
```

This branch will be used to create a PR later.

> [!IMPORTANT]
> Make sure the `post-release-changes` branch is created on `master`, not `rX.Y.Z` or `releases/vX.Y`!

Add the new release to the tables in `etc/apidocmenu.md`.

Edit `README.md` to match the updated `etc/apidocmenu.md`.

(Stable Releases Only) Close the Jira ticket tracking this release with "Documentation Changes" set to "Needed". Fill the "Documentation Changes Summary" field with information requesting updates to:

  - the "Installing the MongoDB C Driver" section of the [Advanced Configuration and Installation Options](https://www.mongodb.com/docs/languages/cpp/cpp-driver/current/installation/advanced/#installing-the-mongodb-c-driver) page
    with any new libmongoc version requirements,
  - the "Driver Status by Family and Version" section of the [home
    page](https://www.mongodb.com/docs/languages/cpp/cpp-driver/current/#driver-status-by-family-and-version), and
  - the [full version](https://github.com/mongodb/docs-cpp/blob/master/snooty.toml) for the C++ Driver documentation pages.

This will generate a DOCSP ticket with instructions to update the C++ Driver docs.

Example (using Jira syntax formatting):

```
* The [Advanced Installation|https://www.mongodb.com/docs/languages/cpp/cpp-driver/current/installation/advanced/#installing-the-mongodb-c-driver] page must be updated with a new requirement: "For mongocxx-X.Y.x, libmongoc A.B.C or later is required.
* The [MongoDB C++ Driver|https://www.mongodb.com/docs/languages/cpp/cpp-driver/current/#driver-status-by-family-and-version] page must be updated: {{{}mongocxx X.Y.x{}}} is now a previous stable release and no longer under active development; {{{}mongocxx X.Y+1.x{}}} is the new current stable release eligible for bug fixes.
* the [full version|https://github.com/mongodb/docs-cpp/blob/master/snooty.toml] for C++ Driver documentation must be updated to {{{}X.Y.Z{}}}.
```

### Publish Updated Documentation

> [!NOTE]
> Some of these commands may take a while to complete.

Set `$LATEST_DOC_TAG` in `etc/generate-latest-apidocs.pl` to the latest release tag.

Commit these changes to the `post-release-changes` branch:

```bash
git commit -am "Prepare to generate rX.Y.Z release documentation"
```

Ensure `doxygen` and `hugo` are locally installed and up-to-date.

```bash
command -V doxygen hugo
```

> [!IMPORTANT]
> The required Doxygen version is defined in `etc/generate-apidocs-from-tag.pl` as `$doxygen_version_required`. If not already present, download the required version from [Doxygen Releases](https://www.doxygen.nl/download.html). Use the `DOXYGEN_BINARY` environment variable to override the default `doxygen` command with a path to a specific Doxygen binary.

Run `git clean -dfx` to restore the repository to a clean state.

> [!WARNING]
> Do NOT run `git clean -dfx` in your local development repository, as it may delete your local development files present in the repository (even if excluded)! Only run this in the command in the separate repository being used for this release!

Configure CMake using `build` as the binary directory. Leave all other configuration variables as their default.

```bash
cmake -S . -B build
```

Test generating Hugo and Doxygen docs locally by building the `docs` target (this command DOES NOT check for the required Doxygen version):

```bash
cmake --build build --target docs
```

Test generating the latest versioned Doxygen docs by building the `doxygen-latest` target (this command DOES checks for the required Doxygen version):

```bash
cmake --build build --target doxygen-latest
```

Verify that the `build/docs/api/mongocxx-X.Y.Z` directory is present and populated. Verify the resulting API doc looks as expected.

Remove all contents of `build/docs/api` before running the next commands.

> [!IMPORTANT]
> Remove all contents of `build/docs/api` before running the next commands.

Generate and deploy the updated documentation to GitHub pages by building the `hugo-deploy` and `doxygen-deploy` targets:

```bash
cmake --build build --target hugo-deploy
cmake --build build --target doxygen-deploy
```

These commands will update the `gh-pages` branch and push the changes to the remote repository.

> [!WARNING]
> Build and release artifacts may still be present in the repository after this step. Do not accidentally commit these files into the repository in the following steps!

### Update Symlinks

> [!IMPORTANT]
> Symlink updates only apply to stable releases! Release candidates and other unstable releases do not require updating symlinks.

Checkout the updated `gh-pages` branch:

```bash
git checkout gh-pages
git pull
```

Update the `api/mongocxx-v3` symlink to refer to the new release version:

```bash
cd api
rm mongocxx-v3
ln -s mongocxx-X.Y.Z mongocxx-v3
```

Double-check that the `current` symlink is pointing to the symlink tracking the latest stable release:

```
current     -> mongocxx-v3
mongocxx-v3 -> mongocxx-X.Y.Z
```

Commit and push this change to the `gh-pages` branch:

```bash
git commit -m "Update symlink for rX.Y.Z"
```

Verify the https://mongocxx.org/api/current/ page has been updated with the new release.

### Update CHANGELOG...

#### ... for a Patch Release

Checkout the updated `releases/vX.Y` branch.

```bash
git checkout releases/vX.Y
git pull
```

Add a section for the next patch release, e.g. following a `1.2.3` release:

```md
## 1.2.4 [Unreleased]

<!-- Will contain entries for the next patch release. -->

## 1.2.3 <!-- Just released. -->

## 1.2.2 <!-- Prior release. -->
```

Commit the changes to the `releases/vX.Y` branch and push the branch to the remote repository (a PR is not required for this step).

Checkout the `post-release-changes` branch.

Sync the entries in the patch release section to be consistent with the entries on the release branch, e.g. following a `1.2.3` release:

```md
## 1.3.0 [Unreleased]

<!-- Will contain entries for the next minor release. -->
<!-- Ensure any existing entries are not removed during the sync. -->

## 1.2.4 [Unreleased]

<!-- Will contain entries for the next patch release. -->

## 1.2.3 <!-- Just released. -->

<!-- Ensure these entries match those in the release. -->

## 1.2.2 <!-- Prior release. -->
```

> [!TIP]
> Use `git restore --source=rX.Y.Z --worktree CHANGELOG.md` to obtain the `CHANGELOG.md` in `rX.Y.Z` as unstaged changes.

#### ... for a Non-Patch Release

Checkout the `post-release-changes` branch.

Add a section for the next minor release, e.g. following a `1.3.0` release:

```md
## 1.4.0 [Unreleased]

<!-- Will contain entries for the next minor release. -->

## 1.3.0 <!-- Just released. -->

## 1.2.2 <!-- Prior release. -->
```

Commit these changes to `post-release-changes`.

### Merge Post-Release Changes

Push the `post-releases-changes` branch to your personal fork repository and create a PR to merge the post-release changes into `master`.

```bash
git remote add origin git@github.com:<username>/mongo-cxx-driver.git
git push -u origin post-release-changes
```

### Announce on Community Forums

Post an announcement to the [Developer Community Forum](https://www.mongodb.com/community/forums/tags/c/announcements/driver-releases/110/cxx) under "Product & Driver Announcements > Driver Releases" and include the "production" and "cxx" tags.

Template:

```md
The MongoDB C++ Driver Team is pleased to announce the availability of [MongoDB C++ Driver X.Y.Z](https://github.com/mongodb/mongo-cxx-driver/releases/tag/rX.Y.Z).

Please note that this version of mongocxx requires [MongoDB C Driver A.B.C](https://github.com/mongodb/mongo-c-driver/releases/tag/A.B.C) or higher.

See the [MongoDB C++ Driver Manual](https://www.mongodb.com/docs/languages/cpp/cpp-driver/current/) and the [Driver Installation Instructions](https://www.mongodb.com/docs/languages/cpp/cpp-driver/current/installation/) for more details on downloading, installing, and using this driver.

NOTE: The mongocxx 3.10.x series does not promise API or ABI stability across patch releases.

Please feel free to post any questions on the MongoDB Community forum in the [Drivers](https://www.mongodb.com/community/forums/c/data/drivers/7) category tagged with [cxx](https://www.mongodb.com/community/forums/tag/cxx). Bug reports should be filed against the [CXX](https://jira.mongodb.org/projects/CXX) project in the MongoDB JIRA. Your feedback on the C++11 driver is greatly appreciated.

Sincerely,

The C++ Driver Team
```

## Packaging

### Homebrew

This requires a macOS machine.
If this is a stable release, update the [mongo-cxx-driver](https://github.com/Homebrew/homebrew-core/blob/master/Formula/mongo-cxx-driver.rb) homebrew formula, using: `brew bump-formula-pr mongo-cxx-driver --url <tarball url>`

Example:
`brew bump-formula-pr mongo-cxx-driver --url https://github.com/mongodb/mongo-cxx-driver/releases/download/r3.7.3/mongo-cxx-driver-r3.7.3.tar.gz`

### vcpkg

Submit a PR or create an issue to update the vc-pkg file for mongo-cxx-driver.
To submit an issue, follow: https://github.com/microsoft/vcpkg/issues/new/choose. Example: https://github.com/microsoft/vcpkg/issues/34984

### Conan

Submit a PR or create an issue to update the Conan recipe for mongo-cxx-driver.
To submit an issue, follow: https://github.com/conan-io/conan-center-index/issues/new/choose/. Example: https://github.com/conan-io/conan-center-index/issues/21006

## Docker Image Build and Publish

> [!IMPORTANT]
> Docker image publication for new releases is currently suspended. See CXX-3042.

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
