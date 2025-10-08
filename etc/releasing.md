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
- A Jira Personal Access Token (PAT)
  - Location: `~/.secrets/jira-token.txt`
  - Format:
    ```
    <jira_token>
    ```
  - See [Jira: Personal Access Tokens (PATs)](https://wiki.corp.mongodb.com/spaces/TOGETHER/pages/218995581/Jira+Personal+Access+Tokens+PATs) for steps to create a token.
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

### Minimum Required MongoDB C Driver Version

Ensure `MONGOC_VERSION_MINIMUM` and related values are updated for the latest minimum required C Driver release.

See the comment accompanying `MONGOC_VERSION_MINIMUM` for a list of other sources to update.

### Coverity

Ensure there are no new or unexpected issues with High severity or greater.

Triage any outstanding issues using the `Issues: By Snapshot | Outstanding Issues` view, create JIRA tickets if necessary, and update issue fields accordingly.

> [!NOTE]
> The "Classification", "Action", and "MongoDB Final Status" fields should always be updated. The "Ext. Reference" field may refer to a JIRA ticket number or an external issue tracker as appropriate. Use the "Notes" field to document rationale for the "MongoDB Final Status" for issues with Medium severity or higher. Add any additional notes for future reference in the "Comments" field.

Verify that all issues listed in the `Issues: By Snapshot | SSDLC Report (v2)` view have been triaged.

All issues with an Impact level of "High" or greater must have a "MongoDB Final Status" of "Fix Committed" and a corresponding JIRA ticket number in the "Ext. Reference" field.

All issues with an Impact level of "Medium" or greater which do not have a "MongoDB Final Status" of "Fix Committed" must document rationale for its current status in the "Notes" field.

### SBOM Lite

Ensure the list of bundled dependencies in `etc/purls.txt` is up-to-date. If not, update `etc/purls.txt`.

If `etc/purls.txt` was updated, update the SBOM Lite document using the following command(s):

```bash
# Artifactory credentials.
. $HOME/.secrets/artifactory-creds.txt

# Output: "Login succeeded!"
podman login --password-stdin --username "${ARTIFACTORY_USER:?}" artifactory.corp.mongodb.com <<<"${ARTIFACTORY_PASSWORD:?}"

# Ensure latest version of SilkBomb is being used.
podman pull artifactory.corp.mongodb.com/release-tools-container-registry-public-local/silkbomb:2.0

# Output: "... writing sbom to file"
podman run -it --rm -v "$(pwd):/pwd" artifactory.corp.mongodb.com/release-tools-container-registry-public-local/silkbomb:2.0 \
  update --refresh --no-update-sbom-version -p "/pwd/etc/purls.txt" -i "/pwd/etc/cyclonedx.sbom.json" -o "/pwd/etc/cyclonedx.sbom.json"
```

Run a patch build which executes the `sbom` task and download the "Augmented SBOM (Updated)" file as `etc/augmented.sbom.json`. Evergreen CLI may be used to schedule only the `sbom` task:

```bash
# Ensure `-p` matches the correct Evergreen project for the current branch!
evergreen patch -y -p mongo-cxx-driver -t all -v sbom -f
```

Commit the updated SBOM documents if there are any substantial changes.

### Augmented SBOM

Ensure the `sbom` task is passing on Evergreen for the relevant release branch.

Review the contents of the new Augmented SBOM and ensure any new or known vulnerabilities with severity "Medium" or greater have a corresponding JIRA ticket (CXX or VULN) that is scheduled to be resolved within its remediation timeline.

Update the [SSDLC Report spreadsheet](https://docs.google.com/spreadsheets/d/1sp0bLjj29xO9T8BwDIxUk5IPJ493QkBVCJKIgptxEPc/edit?usp=sharing) with any updates to new or known vulnerabilities.

Update `etc/third_party_vulnerabilities.md` with any updates to new or known vulnerabilities for third party dependencies that have not yet been fixed by the upcoming release.

Download the "Augmented SBOM (Updated)" file from the latest EVG commit build in the `sbom` task and commit it into the repo as `etc/augmented.sbom.json` (even if the only notable change is the timestamp field).

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

### Dry-run etc/make_release.py

Perform a dry-run of the "Run etc/make_release.py" steps described below.

Verify there are no unexpected errors or issues.

## Release Steps

### Branch Protection Rules

The release process may require creating new branches, new tags, and directly pushing to development branches. These operations are normally restricted by branch protection rules.

When assigned the responsibility of performing a release, submit a request to a repository administrator to be temporarily added to the [dbx-c-cxx-releases](https://github.com/orgs/mongodb/teams/dbx-c-cxx-releases/) team for the duration of the release process. The team member must be added via [MANA](https://mana.corp.mongodb.com/resources/68029673d39aa9f7de6399f9) (the GitHub team is usually empty; no member has the maintainer role).

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

### Pre-Release Changes PR

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

> [!WARNING]
> The upcoming steps may modify the state of the current repository!
> Cloning the updated repository in a new directory is highly recommended.

Create and activate a fresh Python 3 virtual environment with required packages installed using [uv](https://docs.astral.sh/uv/getting-started/installation/):

```bash
# Outside the mongo-cxx-driver-release directory!
export UV_PROJECT_ENVIRONMENT="$HOME/mongo-cxx-driver-release-venv"

# Install required packages into a new virtual environment.
uv sync --frozen --group apidocs --group make_release

# Activate the virtual environment.
source "$UV_PROJECT_ENVIRONMENT/bin/activate"
```

> [!NOTE]
> A new release branch `releases/vX.Y` will be created later as part of post-release steps.

### Run etc/make_release.py

This script performs the following steps:

- create a GPG-signed release tag,
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
- Jira Personal Access Token.
- Artifactory credentials.
- Garasign credentials.

Run the release script with the name of the tag to be created as an argument and
`--dry-run` to test for unexpected errors.

```bash
make_release_args=(
    -r upstream
    --jira-token-file ~/.secrets/jira-token.txt
    --github-token-file ~/.secrets/github-token.txt
)
python ./etc/make_release.py "${make_release_args[@]:?}" --dry-run rX.Y.Z
```

> [!TIP]
> Export environment variables (e.g. `CMAKE_BUILD_PARALLEL_LEVEL`, `CMAKE_GENERATOR`, etc.) to improve the speed of this command.

If an error occurs, inspect logs the script produces, and troubleshoot as
follows:

- Use `--dry-run` to prevent unrecoverable effects.
- Use `--skip-release-tag` to skip creating the release tag when it already exists.
- If building the C driver fails, use an existing C driver build (ensure it is
  the right version) with `--with-c-driver /path/to/c-driver/install`.
- Use `--skip-distcheck` to bypass time consuming checks when building the
  distribution tarball.
- If the script succeeded at creating the distribution tarball, pass it directly
  with `--dist-file ./build/mongo-cxx-driver-rX.Y.Z.tar.gz`.

If all goes well, run the command again without `--dry-run`. This should update Jira and create a draft release on GitHub.

Verify the successful creation of the release draft on GitHub.

### Push the Release Tag

Push the newly-created GPG-signed release tag to the remote repository:

```bash
git push upstream rX.Y.Z
```

### Release on GitHub

Verify the pushed release tag is detected by the release draft (refresh the page if necessary).

Review the contents of the release draft, then publish the release.

### Release on Jira

Navigate to the
[fixVersions page on Jira](https://jira.mongodb.com/plugins/servlet/project-config/CXX/versions?status=unreleased).

Click the "..." next to the relevant version and select "Release".

### Update GitHub Webhook

For a non-patch release, update the [Github Webhook](https://wiki.corp.mongodb.com/display/INTX/Githook) to include the new branch.

Navigate to the [Webhook Settings](https://github.com/mongodb/mongo-cxx-driver/settings/hooks)

Click `Edit` on the hook for `https://githook.mongodb.com/`.

Add the new release branch to the `Payload URL`. Remove unmaintained release branches.

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

### Coverity Report

Export the `Issues: By Snapshot | SSDLC Report (v2)` view as a CSV named `static_analysis_report-X.Y.Z.csv`.

### Upload SSDLC Reports

Upload a copy of the `static_analysis_report-X.Y.Z.csv`, `etc/ssdlc_compliance_report.md`, `etc/third_party_vulnerabilities.md`, and `etc/augmented.sbom.json` files. Rename the files with the version number `-X.Y.Z` suffix in their filenames as already done for other files in this folder.

> [!WARNING]
> Uploading a file into the SSDLC Compliance Files folder is an irreversible action! However, the files may still be renamed. If necessary, rename any accidentally uploaded files to "(Delete Me)" or similar.

Four new files should be present in the [SSDLC Compliance Files](https://drive.google.com/drive/folders/1_qwTwYyqPL7VjrZOiuyiDYi1y2NYiClS) folder following a release `X.Y.Z`:

```
augmented.sbom-X.Y.Z.json
ssdlc_compliance_report-X.Y.Z.md
static_analysis_report-X.Y.Z.csv
third_party_vulnerabilities-X.Y.Z.md
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
git push upstream releases/vX.Y
```

The new branch should be continuously tested on Evergreen. Update the "Display Name" and "Branch Name" of the [mongo-cxx-driver-latest-release Evergreen project](https://spruce.mongodb.com/project/mongo-cxx-driver-latest-release/settings/general) to refer to the new release branch.

Update `etc/cyclonedx.sbom.json` with a new unique serial number for the next upcoming patch release (e.g. for `1.3.1` following the release of `1.3.0`):

```bash
# Artifactory credentials.
. $HOME/.secrets/artifactory-creds.txt

# Output: "Login succeeded!"
podman login --password-stdin --username "${ARTIFACTORY_USER:?}" artifactory.corp.mongodb.com <<<"${ARTIFACTORY_PASSWORD:?}"

# Ensure latest version of SilkBomb is being used.
podman pull artifactory.corp.mongodb.com/release-tools-container-registry-public-local/silkbomb:2.0

# Output: "... writing sbom to file"
podman run -it --rm -v "$(pwd):/pwd" artifactory.corp.mongodb.com/release-tools-container-registry-public-local/silkbomb:2.0 \
  update --refresh --generate-new-serial-number -p "/pwd/etc/purls.txt" -i "/pwd/etc/cyclonedx.sbom.json" -o "/pwd/etc/cyclonedx.sbom.json"
```

Update `etc/augmented.sbom.json` by running a patch build which executes the `sbom` task as described above in [SBOM Lite](#sbom-lite).

Commit and push these changes to the `releases/vX.Y` branch.

### Update Snyk

> [!IMPORTANT]
> Run the Snyk commands in a fresh clone of the post-release repository to avoid existing build and release artifacts from affecting Snyk.

Checkout the new release tag.

Configure and build the CXX Driver (do not reuse an existing C Driver installation; use the auto-downloaded C Driver sources instead):

```bash
cmake -S . -B build
cmake --build build
```

Then run:

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
  --exclude=extras # CXX-3042
)
snyk test "${snyk_args[@]:?}" --print-deps

# Create a new Snyk target reference for the new release tag.
snyk monitor "${snyk_args[@]:?}"
```

Verify the new Snyk target reference is present in the [Snyk project targets list](https://app.snyk.io/org/dev-prod/projects?groupBy=targets&before&after&searchQuery=mongo-cxx-driver&sortBy=highest+severity&filters[Show]=&filters[Integrations]=cli&filters[CollectionIds]=) for `mongodb/mongo-cxx-driver`.

### Post-Release Changes

Create and checkout a new branch `post-release-changes` relative to `master` to contain documentation updates following the new release:

```bash
git fetch upstream
git checkout -b post-release-changes upstream/master
```

This branch will be used to create a PR later.

> [!IMPORTANT]
> Make sure the `post-release-changes` branch is created on `master`, not `rX.Y.Z` or `releases/vX.Y`!

In `etc/apidocmenu.md`, update the list of versions under "Driver Documentation By Version" and the table under "Driver Development Status" with a new entry corresponding to this release.

In `README.md`, sync the "Driver Development Status" table with the updated table from `etc/apidocmenu.md`.

Update `etc/cyclonedx.sbom.json` with a new unique serial number for the next upcoming non-patch release (e.g. for `1.4.0` following the release of `1.3.0`):

```bash
# Artifactory credentials.
. $HOME/.secrets/artifactory-creds.txt

# Output: "Login succeeded!"
podman login --password-stdin --username "${ARTIFACTORY_USER:?}" artifactory.corp.mongodb.com <<<"${ARTIFACTORY_PASSWORD:?}"

# Ensure latest version of SilkBomb is being used.
podman pull artifactory.corp.mongodb.com/release-tools-container-registry-public-local/silkbomb:2.0

# Output: "... writing sbom to file"
podman run -it --rm -v "$(pwd):/pwd" artifactory.corp.mongodb.com/release-tools-container-registry-public-local/silkbomb:2.0 \
  update --refresh --generate-new-serial-number -p "/pwd/etc/purls.txt" -i "/pwd/etc/cyclonedx.sbom.json" -o "/pwd/etc/cyclonedx.sbom.json"
```

Update `etc/augmented.sbom.json` by running a patch build which executes the `sbom` task as described above in [SBOM Lite](#sbom-lite).

Commit these changes to the `post-release-changes` branch:

```bash
git commit -m "Post-release changes"
```

### Create Documentation Tickets

(Stable Releases Only) Close the Jira ticket tracking this release with "Documentation Changes" set to "Needed". Fill the "Documentation Changes Summary" field with information requesting updates to:

  - the "Installing the MongoDB C Driver" section of the [Advanced Configuration and Installation Options](https://www.mongodb.com/docs/languages/cpp/cpp-driver/current/installation/advanced/#installing-the-mongodb-c-driver) page
    with any new C Driver version requirements,
  - the "Driver Status by Family and Version" section of the [home
    page](https://www.mongodb.com/docs/languages/cpp/cpp-driver/current/#driver-status-by-family-and-version), and
  - the [full version](https://github.com/mongodb/docs-cpp/blob/master/snooty.toml) for the C++ Driver documentation pages.

This will generate a DOCSP ticket with instructions to update the C++ Driver docs.

Example (using Jira syntax formatting):

```
* The [Advanced Installation|https://www.mongodb.com/docs/languages/cpp/cpp-driver/current/installation/advanced/#installing-the-mongodb-c-driver] page must be updated with a new requirement: "For mongocxx-X.Y.x, mongoc A.B.C or later is required."
* The [MongoDB C++ Driver|https://www.mongodb.com/docs/languages/cpp/cpp-driver/current/#driver-status-by-family-and-version] page must be updated: {{{}mongocxx X.Y.x{}}} is now a previous stable release and no longer under active development; {{{}mongocxx X.Y+1.x{}}} is the new current stable release eligible for bug fixes.
* the [full version|https://github.com/mongodb/docs-cpp/blob/master/snooty.toml] for C++ Driver documentation must be updated to {{{}X.Y.Z{}}}.
```

### Publish Updated Documentation

> [!NOTE]
> Some of these commands may take a while to complete.

Set `$LATEST_VERSION` in `etc/generate-latest-apidocs.sh` to the latest release version.

Change the version number for `SITEMAP_URL` in `Doxyfile` to the latest release version.

Commit these changes to the `post-release-changes` branch:

```bash
git commit -am "Prepare to generate rX.Y.Z release documentation"
```

Ensure `doxygen` and `hugo` are locally installed and up-to-date.

```bash
command -V doxygen hugo
```

> [!IMPORTANT]
> The required Doxygen version is defined in `etc/generate-latest-apidocs.sh` as `$DOXYGEN_VERSION_REQUIRED`. If not already present, download the required version from [Doxygen Releases](https://www.doxygen.nl/download.html). Use the `DOXYGEN_BINARY` environment variable to override the default `doxygen` command with a path to a specific Doxygen binary.

Run `git clean -dfx` to restore the repository to a clean state.

> [!WARNING]
> Do NOT run `git clean -dfx` in your local development repository, as it may delete your local development files present in the repository (even if normally ignored by git)! Only run this in the command in the separate repository being used for this release!

Configure CMake using `build` as the binary directory. Leave all other configuration variables as their default.

```bash
cmake -S . -B build
```

Test generating both Hugo and Doxygen docs locally by building the `docs` target:

```bash
export DOXYGEN_BINARY=<path/to/doxygen> # Optional. For binary version compatibility.

cmake --build build --target docs
```

Test generating the latest versioned Doxygen docs by building the `doxygen-latest` target:

```bash
export DOXYGEN_BINARY=<path/to/doxygen> # Optional. For binary version compatibility.

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

### Update gh-pages

> [!IMPORTANT]
> Symlink updates only apply to stable releases! Release candidates and other unstable releases do not require updating symlinks.

Checkout the updated `gh-pages` branch:

```bash
git checkout gh-pages
git pull
```

Update the `api/mongocxx-vX` symlink to refer to the new release version, e.g.:

```bash
cd api
rm mongocxx-vX
ln -s mongocxx-X.Y.Z mongocxx-vX
```

Double-check that the `current` symlink is valid and points to the symlink tracking the latest stable release of the most recent major release version, e.g.:

```
current     -> mongocxx-v4
mongocxx-v4 -> mongocxx-4.Y.Z
mongocxx-v3 -> mongocxx-3.Y.Z
```

Add a new entry to the `sitemap_index.xml` file referencing the sitemap for `api/mongocxx-X.Y.Z`.
Set `<lastmod>` for both the new entry and the `/current` sitemap entry to the current date:

```xml
...
<!-- API Documentation Pages. -->
<sitemap>
  <loc>https://mongocxx.org/api/current/sitemap.xml</loc>
  <lastmod>YYYY-MM-DD</lastmod>
</sitemap>
<sitemap>
  <loc>https://mongocxx.org/api/mongocxx-X.Y.Z/sitemap.xml</loc>
  <lastmod>YYYY-MM-DD</lastmod>
</sitemap>
...
```

Commit and push these change to the `gh-pages` branch:

```bash
git commit -m "Update symlink and sitemap for rX.Y.Z"
```

Wait for [GitHub Actions](https://github.com/mongodb/mongo-cxx-driver/actions) to finish deploying the updated pages.

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

Checkout the `releases/vX.Y` release branch.

```bash
git checkout releases/vX.Y
git pull
```

Add a section for the next patch release, e.g. following a `1.2.0` release:

```md
## 1.2.1 [Unreleased]

<!-- Will contain entries for the next patch release. -->

## 1.2.0 <!-- Just released. -->

## 1.1.0 <!-- Prior release. -->
```

Commit the changes to the `releases/vX.Y` branch and push the branch to the remote repository (a PR is not required for this step).

Checkout the `post-release-changes` branch.

Add a section for the next minor release, e.g. following a `1.3.0` release:

```md
## 1.4.0 [Unreleased]

<!-- Will contain entries for the next minor release. -->

## 1.3.0 <!-- Just released. -->

## 1.2.2 <!-- Prior release. -->
```

Commit these changes to `post-release-changes`.

```bash
git commit -m "Add CHANGELOG section for the next minor release"
```

### Merge Post-Release Changes

Push the `post-releases-changes` branch to your personal fork repository and create a PR to merge the post-release changes into `master`.

```bash
git remote add origin git@github.com:<username>/mongo-cxx-driver.git
git push -u origin post-release-changes
```

### Announce on Community Forums

Post an announcement to the [Developer Community Forum](https://www.mongodb.com/community/forums/tags/c/announcements/driver-releases/110/cxx) under "Product & Driver Announcements > Driver Releases" and include the "production" and "cxx" tags.

Template Title:

```
MongoDB C++11 Driver X.Y.Z Released
```

Template Body:

```md
The MongoDB C++ Driver Team is pleased to announce the availability of [MongoDB C++ Driver X.Y.Z](https://github.com/mongodb/mongo-cxx-driver/releases/tag/rX.Y.Z).

Please note that this version of mongocxx requires [MongoDB C Driver A.B.C](https://github.com/mongodb/mongo-c-driver/releases/tag/A.B.C) or newer.

See the [MongoDB C++ Driver Manual](https://www.mongodb.com/docs/languages/cpp/cpp-driver/current/) and the [Driver Installation Instructions](https://www.mongodb.com/docs/languages/cpp/cpp-driver/current/installation/) for more details on downloading, installing, and using this driver.

NOTE: The mongocxx 3.10.x series does not promise API or ABI stability across patch releases.

Please feel free to post any questions on the MongoDB Community forum in the [Drivers](https://www.mongodb.com/community/forums/c/data/drivers/7) category tagged with [cxx](https://www.mongodb.com/community/forums/tag/cxx). Bug reports should be filed against the [CXX](https://jira.mongodb.org/projects/CXX) project in the MongoDB JIRA. Your feedback on the C++11 driver is greatly appreciated.

Sincerely,

The C++ Driver Team
```

### Update the Release Info Spreadsheet

Add an entry to the [C/C++ Release Info](https://docs.google.com/spreadsheets/d/1yHfGmDnbA5-Qt8FX4tKWC5xk9AhzYZx1SKF4AD36ecY) spreadsheet documenting the date, release version, author (of the release), and additional comments.

### Update Branch Protection Rules

Notify a repository administrator that the previous stable release branch (which will no longer recieve any further updates) may now be "locked" by adding it to the list of branches in the "Restrict Branch Updates" ruleset.

Once the release process is complete, use [MANA](https://mana.corp.mongodb.com/resources/68029673d39aa9f7de6399f9) to remove yourself from the [dbx-c-cxx-releases](https://github.com/orgs/mongodb/teams/dbx-c-cxx-releases/) team or request a repository administrator to remove you instead.

## Packaging

### vcpkg

Submit a PR or create an issue to update the vc-pkg file for mongo-cxx-driver.
To submit an issue, follow: https://github.com/microsoft/vcpkg/issues/new/choose (Request an update to an existing port). Example: [r3.10.2](https://github.com/microsoft/vcpkg/issues/39539).

Include a note communicating new minimum C Driver version requirements.

### Conan

Submit a PR or create an issue to update the Conan recipe for mongo-cxx-driver.
To submit an issue, follow: https://github.com/conan-io/conan-center-index/issues/new/choose/ (Package: New Version). Example: [r3.10.2](https://github.com/conan-io/conan-center-index/issues/24451).

Include a note communicating new minimum C Driver version requirements.

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

> [!IMPORTANT]
> These instructions should be kept in sync with the corresponding C driver
> release process documentation located in the `docs/dev/debian.rst` file in the
> C driver repository.

#### Build

1. Change to the packaging branch, `git checkout debian/unstable`, and make sure
   the working directorty is clean, `git status`, and up-to-date, `git pull`.
2. Because it is possible to have divergences between release branches, some
   special procedures are needed. Execute the following sequence of commands
   (substituting version numbers as appropriate):

```
$ git merge --no-commit --no-ff r3.xx.y     # may result in conflicts
$ git checkout HEAD -- debian               # ensures debian/ dir is preserved
$ git add .                                 # prepare to resolve conflicts
$ git checkout --no-overlay r3.xx.y -- . ':!debian' # resolve conflicts
$ git add .
$ git commit
```

3. Verify that there are no extraneous differences from the release tag,
   `git diff r3.xx.y..HEAD --stat -- . ':!debian'`; the command should produce
   no output, and if any output is shown then that indicates differences in
   files outside the `debian/` directory.
4. If there were any files outside the `debian/` directory listed in the last
   step then something has gone wrong. Discard the changes on the branch and
   start again.
5. Create a new changelog entry (use the command `dch -i` to ensure proper
   formatting), then adjust the version number on the top line of the changelog
   as appropriate.
6. Make any other necessary changes to the Debian packaging components (e.g.,
   update to standards version, dependencies, descriptions, etc.) and make
   relevant entries in `debian/changelog` as needed.
7. Use `git add` to stage the changed files for commit (only files in the
   `debian/` directory should be committed), then commit them (the `debcommit`
   utility is helpful here).
8. Build the package with `gbp buildpackage` and inspect the resulting package
   files (at a minimum use `debc` on the `.changes` file in order to confirm
   files are installed to the proper locations by the proper packages and also
   use `lintian` on the `.changes` file in order to confirm that there are no
   unexpected errors or warnings; the `lintian` used for this check should
   always be the latest version as it is found in the unstable distribution).
9. If any changes are needed, make them, commit them, and rebuild the package.

> [!IMPORTANT]
> It may be desirable to squash multiple commits down to a single commit before building the final packages.

10. Mark the package ready for release with the `dch -Dexperimental -r` command
    and commit the resulting changes (after inspecting them),
    `git commit debian/changelog -m 'mark ready for release'`.
11. Build the final packages. Once the final packages are built, they can be
    signed and uploaded and the version can be tagged using the `--git-tag`
    option of `gbp buildpackage`. The best approach is to build the packages,
    prepare everything and then upload. Once the archive has accepted the
    upload, then execute
    `gbp buildpackage --git-tag --git-tag-only --git-sign-tags` and push the
    commits on the `debian/unstable` branch as well as the new signed tag.

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
