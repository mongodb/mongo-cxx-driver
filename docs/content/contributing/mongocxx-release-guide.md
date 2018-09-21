+++
date = "2016-08-15T16:11:58+05:30"
title = "Releasing the mongocxx driver"
[menu.main]
  weight = 20
  parent="contributing"
+++

**NOTE**: To follow these instructions, be very careful which branch you're
working on:

* Version bump/tag/post-bump commits go on the branch corresponding to the release.
* Documentation updates go on the `master` branch.

The instructions below will remind you about branches.

## Update fixVersion Status in Jira tickets

* Ensure that all tickets under the version to be released are in
  'Closed' status on the C++ Driver [roadmap
  page](https://jira.mongodb.org/browse/CXX/?selectedTab=com.atlassian.jira.jira-projects-plugin:roadmap-panel)
* If not, bulk change **Open** tickets that will **NOT** be in the release to a new
  version (create it if necessary):

## Audit Jira ticket titles and types for use in release notes

* From the [roadmap page](https://jira.mongodb.org/browse/CXX/?selectedTab=com.atlassian.jira.jira-projects-plugin:roadmap-panel)
  click the "Release Notes" link to see a summary of tickets to be included
  in release notes.
* Update the ticket type and title as appropriate.  User-facing issues
  should generally be either "Bug" or "New Feature".  Non-user facing
  issues should generally be "Task" tickets (and will be omitted later, so
  you can ignore them here).

## Create a BUMP commit

This commit essentially removes the -pre (e.g. 1.0.0-rc79-pre ->
1.0.0-rc79) and creates what is considered the final commit in the release.

*  If you haven't already, clone the mongodb-cxx-driver repository

```
git clone git@github.com:mongodb/mongo-cxx-driver.git
cd mongo-cxx-driver
```

* Otherwise, pull the latest code down from the origin with `git pull`
* Checkout the branch corresponding to the release
* Update the following files (changing version numbers to remove the "-pre"
  for the MONGOCXX_VERSION_EXTRA and BSONCXX_VERSION_EXTRA variables):
    * [`src/mongocxx/CMakeLists.txt`](https://github.com/mongodb/mongo-cxx-driver/blob/master/src/mongocxx/CMakeLists.txt#L30)
    * [`src/bsoncxx/CMakeLists.txt`](https://github.com/mongodb/mongo-cxx-driver/blob/master/src/bsoncxx/CMakeLists.txt#L22)
* Commit with message "BUMP rx.y.z[-rcq]", e.g.:

```
git commit -am "BUMP r3.0.1"
git push origin <release-branch>
```

Pushing the BUMP commit to the origin will kick off builds in Evergreen
and Travis CI. Wait for both builds to finish and pass, re-running any
timeout/spot failures.

Meanwhile, test the branch locally.

## Tag the Release

* Create an tag for the commit that will serve as the release (or release candidate), eg:

```
git tag r3.0.1
git push origin r3.0.1
```

## Draft & Publish GitHub Release Notes

* Go to the [GitHub releases page](https://github.com/mongodb/mongo-cxx-driver/releases).
* Copy the previous announcement text for reuse.
* Click "Draft a New Release" and set the title and tag.  Check the
  "This is a pre-release" checkbox if appropriate.
* Paste the previous announcement and modify with release-specific information
    * Go back to the "Release Notes" page in Jira; copy the HTML and paste it into
      the GitHub Release Notes markdown.  Delete the "Task" section and any
      other information that doesn't need to be in the release notes.
* Preview the release text and proofread it.
* Publish the release.

## Release the Version in Jira

* Go
  [here](https://jira.mongodb.org/plugins/servlet/project-config/CXX/versions)
* Click the cog next to the version you are about to release and select "Release"
* Follow the dialogs/wizards and whatnot, setting the release date to the current date
* If the next version does not exist, create it.  E.g.:
    * If releasing 3.1.1, create 3.2.2.
    * If releasing 3.4.0-rc0, create 3.4.0-rc1.
    * If releasing 3.4.0, create 3.4.1 **and** 3.5.0-rc0.

## Create the Next Version commit

* Make sure you're on the branch corresponding to the release.
* Update the following files (changing version numbers to bump to the next version number and add the "-pre" suffix for the MONGOCXX_VERSION_EXTRA and BSONCXX_VERSION_EXTRA variables):
    * [`src/mongocxx/CMakeLists.txt`](https://github.com/mongodb/mongo-cxx-driver/blob/master/src/mongocxx/CMakeLists.txt#L30)
    * [`src/bsoncxx/CMakeLists.txt`](https://github.com/mongodb/mongo-cxx-driver/blob/master/src/bsoncxx/CMakeLists.txt#L22)
* Commit with message "post rx.y.z[-rcq]", e.g.:

```
git commit -am "post r3.0.1"
git push origin <release-branch>
```

## Possibly update the releases/stable branch

We use the `releases/stable` branch to track the best 'stable' release for
users who install from the git repository.

After any stable release (i.e. not an alpha, beta, RC, etc. release), check
out the `releases/stable` branch, reset it to the new release tag, and
force push it to the repo. E.g.:

```
git checkout releases/stable
git reset --hard r3.0.1
git push -f origin releases/stable
```

## Generate and Publish Documentation

Documentation generation must be run after the release tag has been made
and pushed.

Documentation updates always go on the **master** branch.

* Checkout the master branch.

* Edit `etc/apidocmenu.md` and add the released version in the 'mongocxx'
  column following the established pattern.  If this is a major release
  (x.y.0), revise the entire document as needed.
    * If revised substantially, also edit `docs/content/index.md` and `README.md` to match.

* Edit `etc/generate-all-apidocs.pl` and add the new release version to
  the `@DOC_TAGS` array, following the established pattern.

* Edit `docs/content/mongocxx-v3/installation.md` and update `Step 3` to
  reflect the latest version to download.  (Generally, search for the last
  version number and update it.)

* Commit these changes.

`git commit -am "Prepare to generate r3.0.2 release documentation"`

* Ensure you have `doxygen` and `hugo` installed and up to date.  `hugo`
  must be at least version v0.16.

* Change to the repo top directory and run `git clean -dxf` to clear out
  all extraneous files.

* Change to the `build` directory and run cmake as you usually would.

* To test documentation generation without deploying, run the `docs` build
  target.  E.g. `ninja docs`.  If this works, your documentation tools are
  correctly installed.  Note that this only generates hugo docs and a
  single set of doxygen docs for the current repo.  To test generation of
  all docs for all tags, use the `doxygen-all` target and be prepared to
  wait a while.

* To generate and deploy documentation to GitHub Pages, execute both the
  `hugo-deploy` and `doxygen-deploy` targets.  The doxygen build will take
  a long time.

* If the release was *not* a release-candidate, update symlinks
    * Check out the "gh-pages" branch and `git pull` the deployed docs.
    * If the release was *not* a release-candidate, update the
      api/mongocxx-v3 symlink to point to the newly released version.  If a
      major version bump has occurred, revise the symlink structure as
      needed.  Make sure 'current' always points to a *symlink* tracking
      the latest release branch.
    * Commit and push the symlink change:  `git commit -am "Update symlink for r3.0.3"`

* Wait a minute and verify the docs site has been updated.

* Return to the original branch.

## File a DOCS ticket

If there is any change to the driver-server compatibility matrix or
language compatibility matrix file a [DOCS
ticket](https://jira.mongodb.org/browse/DOCS/).  This generally will only
apply to an x.y.0 release.

## Email Google Groups

* Send it to mongodb-announce@googlegroups.com AND mongodb-user@googlegroups.com

### Release Candidate Template:

***Subject: MongoDB C++11 Driver 3.1.0-rc0 Released***

The MongoDB C++ Driver Team is pleased to announce the availability of
[mongocxx-3.1.0-rc0](https://github.com/mongodb/mongo-cxx-driver/releases/tag/r3.1.0-rc0),
the first release candidate in the 3.1.x series of the MongoDB C++11
Driver.  This release candidate has been published for testing and is not
recommended for production.

This release provides support for new features in MongoDB 3.4.

Please note that this version of mongocxx requires the [MongoDB C driver
1.5.0](https://github.com/mongodb/mongo-c-driver/releases/tag/1.5.0).

See the [MongoDB C++ Driver
Manual](https://mongodb.github.io/mongo-cxx-driver/) and the [Driver
Installation
Instructions](https://mongodb.github.io/mongo-cxx-driver/mongocxx-v3/installation/)
for more details on downloading, installing, and using this driver.

NOTE: The mongocxx 3.1.x series does not promise API or ABI stability
across patch releases.

Please feel free to post any questions to the
[mongodb-user](https://groups.google.com/forum/#!forum/mongodb-user)
mailing list.  Bug reports should be filed against the
[CXX](https://jira.mongodb.org/browse/CXX) project in the MongoDB JIRA.
Your feedback on the C++11 driver is greatly appreciated.

Thank you,<br/>
The C++ Driver Team

### GA Release Template:

***Subject: MongoDB C++11 Driver 3.1.0 Released***

The MongoDB C++ Driver Team is pleased to announce the availability of
[mongocxx-3.1.0](https://github.com/mongodb/mongo-cxx-driver/releases/tag/r3.1.0).
This release provides support for new features in MongoDB 3.4.

Please note that this version of mongocxx requires the [MongoDB C driver
1.5.0](https://github.com/mongodb/mongo-c-driver/releases/tag/1.5.0).

See the [MongoDB C++ Driver
Manual](https://mongodb.github.io/mongo-cxx-driver/) and the [Driver
Installation
Instructions](https://mongodb.github.io/mongo-cxx-driver/mongocxx-v3/installation/)
for more details on downloading, installing, and using this driver.

NOTE: The mongocxx 3.1.x series does not promise API or ABI stability
across patch releases.

Please feel free to post any questions to the
[mongodb-user](https://groups.google.com/forum/#!forum/mongodb-user)
mailing list.  Bug reports should be filed against the
[CXX](https://jira.mongodb.org/browse/CXX) project in the MongoDB JIRA.
Your feedback on the C++11 driver is greatly appreciated.

Thank you,<br/>
The C++ Driver Team

