+++
date = "2016-08-15T16:11:58+05:30"
title = "Releasing the legacy driver"
[menu.main]
  weight = 105
  parent="contributing"
+++

If you're reading this, chances are you're about to issue a release for a
new version of the legacy C++ driver.  Congratulations!

## Before Getting Started

To perform the release process, you will need the following:

* `git`, `python`, `doxygen` installed on your local machine.
* Push access to the `mongodb/mongo-cxx-driver` repository on GitHub.
* Push access to the `10gen/apidocs` repository on GitHub.
* Administrative access on the [C++ Driver JIRA project](https://jira.mongodb.org/browse/CXX).

This guide assumes that you have a local clone of the `mongo-cxx-driver`
and `apidocs` repositories, and that your GitHub remote is configured with
remote name `origin`.

In addition, this guide assumes that you are about to issue a production
release named `legacy-x.y.z`.  The process for issuing a release candidate
is nearly identical, with two exceptions:

- The new version string will be in format `legacy-x.y.z-rcw`, instead of
  `legacy-x.y.z`.  In this guide, replace all instances of the latter with
  the former.
- The copy for the release notes and announcement will need to indicate
  that the release is not for production use.

Lastly, this guide also assumes that the following release is is named
`legacy-x.y.(z+1)`.  The name of the following release may differ from
this.

## Confirm All Outstanding Issues Resolved in JIRA

- On the [project roadmap
  panel](https://jira.mongodb.org/browse/CXX/?selectedTab=com.atlassian.jira.jira-projects-plugin:roadmap-panel)
  in JIRA, ensure that all tickets marked for `legacy-x.y.z` are resolved.
  Assuming there are 10 issues in the release, you should see the text "10
  of 10 issues have been resolved" on this page.

## Create and Push the BUMP Commit

* Change directories to your local clone of `mongo-cxx-driver`, and use
  `git status` to confirm a clean working tree, and that you have the
  `legacy` branch checked out.
* Make edits to the following files.  You will be changing the development
  version string (`legacy-x.y.z-rcw-pre`) in these files to the release
  version string (`legacy-x.y.z`):
        * `SConstruct`: update value of `mongoclientVersion`.
        * `etc/doxygen/config`: update value of `PROJECT_NUMBER`.
* Commit and push the bump commit, as follows:

	```
	git add SConstruct etc/doxygen/config
	git commit -m "BUMP legacy-x.y.z"
	git push origin legacy
	```

Pushing the BUMP commit to the origin will kick off builds in Evergreen,
Travis CI and AppVeyor. Wait for all three builds to finish and pass.

## Tag the Release

* From the same directory, create and push a tag for the commit that will serve as the release.

	```
	git tag legacy-x.y.z
	git push origin legacy-x.y.z
	```

## Generate and Publish Documentation

> Warning: this section is obsolete

* In your local clone of the `apidocs` repo, build and publish the release
  documentation as follows.  The `build.py` script will modify existing
  files, and also create files in a new directory named `cxx/legacy-x.y.z`.

	```
	python build.py cxx
	git add cxx/
	git add -u
	git commit -m "Documentation for C++ driver release legacy-x.y.z"
	git push origin master
	```

Within an hour, a regularly scheduled task will automatically deploy the
new documentation at the [legacy C++ driver API documentation
page](http://api.mongodb.com/cxx/).

## Bulk Transition All Tickets Targeted for Release to "Closed"

- Re-visit the [project roadmap
  panel](https://jira.mongodb.org/browse/CXX/?selectedTab=com.atlassian.jira.jira-projects-plugin:roadmap-panel)
  in JIRA.
- Under the legacy-x.y.z version, click the link "X issues" on the left-hand side.  This will bring you to the search page that displays all tickets targeted for the version about to be released.
- Select "Tools => Bulk Change" from the upper-right hand corner of the search page.
- In the "Choose Issues" dialog, select all issues.
- In the "Choose Operation" dialog, select "Transition Issues".
- In the "Edit Fields" dialog, click "Next" without modifying any fields.
- Confirm the change.

## Release the Version in JIRA

* Visit the [versions administrative
  panel](https://jira.mongodb.org/plugins/servlet/project-config/CXX/versions)
  in JIRA.
* Hover over the version to be released.  A cog icon will appear on the right-hand side of the page.
* Click the cog icon, and select "Release" from the drop-down.  A pop-up will appear, and it should contain no warnings about unresolved issues.
* Set "Release date" to the current date.
* Confirm the release.

## Create the Next Version in JIRA

* From the [versions administrative
  panel](https://jira.mongodb.org/plugins/servlet/project-config/CXX/versions),
  determine whether or not the version corresponding to the next release
  already exists.
* If the version already exists, continue to the next step.
* If the version does not exist, enter the new version's Name and Description at the prompt towards the top of the page.  Select the current date as the next version's start date, and pick a release date.  Typically, the legacy C++ driver is released only a small handful of times each year, so the release date for the new version should likely be at least a couple of months out (barring extraordinary circumstances).

## Draft & Publish GitHub Release Notes

* Visit the [GitHub releases
  page](https://github.com/mongodb/mongo-cxx-driver/releases).
* Click "Draft a New Release".
* Select `legacy-x.y.z` as the tag.  The page should indicate that this tag is recognized.
* Enter "MongoDB Legacy C++ Driver x.y.z" as the release title.
* Draft the release notes.  Typically, release notes have two sections:
	* In the first section, describe in a few sentences what this release is all about.  Mention whether this is a major release or a minor patch release, and call out any notable tickets resolved and anything else that users may want to know about.
	* In the second section, paste in a formatted list of tickets that were resolved in this release.  You can copy-paste this formatted list straight from JIRA's automatically-generated release notes, which you can find by clicking on the "Release Notes" link under the release version at the [project roadmap panel](https://jira.mongodb.org/browse/CXX/?selectedTab=com.atlassian.jira.jira-projects-plugin:roadmap-panel).
* Select "Publish release".

## Send Announcement Email

* Draft an email to mongodb-announce@googlegroups.com announcing the
  release, with subject "Legacy C++ driver legacy-x.y.z released."
* Use the below text as a template, and feel free to include any extra information that seems appropriate for an announcement email:

> The C++ Driver Team is excited to announce the availability of the [legacy-x.y.z](https://github.com/mongodb/mongo-cxx-driver/releases/tag/legacy-x.y.z) release of the Legacy C++ Driver.
>
> This is a stable release.  Please review the [release notes](https://github.com/mongodb/mongo-cxx-driver/releases/tag/legacy-x.y.z) for details on what has changed in this release. The legacy-x.y.z release is a recommended upgrade for all users of the LAST_RELEASE and older drivers.
>
> You can obtain the driver source from GitHub, either under the [legacy-x.y.z](https://github.com/mongodb/mongo-cxx-driver/tree/legacy-x.y.z) tag or from the [releases page](https://github.com/mongodb/mongo-cxx-driver/releases). The legacy C++ driver is a source level distribution and does not offer a stable ABI across releases.
>
> Please feel free to post any questions to the [mongodb-user](https://groups.google.com/forum/#!forum/mongodb-user) mailing list. Bug reports should be filed against the [CXX](https://jira.mongodb.org/browse/CXX) project in the MongoDB JIRA.
>
> Thank you,
>
> The C++ Driver Team

## Create and Push the Next Version Commit

* Change directories back to your clone of `mongo-cxx-driver`.  Edit the following files:
	* `SConstruct`: change value of `mongoclientVersion` from `legacy-x.y.z` to `legacy-x.y.(z+1)-rc0-pre`.
	* `etc/doxygen/config`: change value of `PROJECT_NUMBER` from `legacy-x.y.z` to `legacy-x.y.(z+1)-rc0-pre`.
	* `README.md`: change both the link text and link address to refer to `legacy-x.y.z`.
* Commit and push the next version commit, as follows:

	```
	git add SConstruct etc/doxygen/config README.md
	git commit -m "post legacy-x.y.z"
	git push origin legacy
	```

## Relax

You're done!
