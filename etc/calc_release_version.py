#!/usr/bin/env python

#
# Copyright 2018-present MongoDB, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

"""
A script that calculates the release version number (based on the current Git
branch and/or recent tags in history) to assign to a tarball generated from the
current Git commit.
"""

import datetime
import re
import subprocess
import sys
from distutils.version import LooseVersion

DEBUG = len(sys.argv) > 1 and '-d' in sys.argv
if DEBUG:
    print('Debugging output enabled.')

# This options indicates to output the next minor release version
NEXT_MINOR = len(sys.argv) > 1 and '--next-minor' in sys.argv

RELEASE_TAG_RE = re.compile('r(?P<ver>(?P<vermaj>[0-9]+)\\.(?P<vermin>[0-9]+)'
                            '\\.(?P<verpatch>[0-9]+)(?:-(?P<verpre>.*))?)')
RELEASE_BRANCH_RE = re.compile('(?:(?:refs/remotes/)?origin/)?(?P<brname>releases/v'
                               '(?P<vermaj>[0-9]+)\\.(?P<vermin>[0-9]+))')

def check_output(args):
    """
    Delegates to subprocess.check_output() if it is available, otherwise
    provides a reasonable facsimile.
    """
    if 'check_output' in dir(subprocess):
        out = subprocess.check_output(args)
    else:
        proc = subprocess.Popen(args, stdout=subprocess.PIPE)
        out, err = proc.communicate()
        ret = proc.poll()
        if ret:
            raise subprocess.CalledProcessError(ret, args[0], output=out)

    if type(out) is bytes:
        # git isn't guaranteed to always return UTF-8, but for our purposes
        # this should be fine as tags and hashes should be ASCII only.
        out = out.decode('utf-8')

    return out


def check_head_tag():
    """
    Checks the current HEAD to see if it has been tagged with a tag that matches
    the pattern for a release tag.  Returns release version calculated from the
    tag, or None if there is no matching tag associated with HEAD.
    """

    found_tag = False
    version_loose = LooseVersion('0.0.0')

    # have git tell us if any tags that look like release tags point at HEAD;
    # based on our policy, a commit should never have more than one release tag
    tags = check_output(['git', 'tag', '--points-at', 'HEAD', '--list', 'r*']).split()
    tag = ''
    if len(tags) == 1:
        tag = tags[0]
    elif len(tags) > 1:
        raise Exception('Expected 1 or 0 tags on HEAD, got: {}'.format(tags))

    release_tag_match = RELEASE_TAG_RE.match(tag)
    if release_tag_match:
        new_version_loose = LooseVersion(release_tag_match.group('ver'))
        if new_version_loose > version_loose:
            if DEBUG:
                print('HEAD release tag: ' + release_tag_match.group('ver'))
            version_loose = new_version_loose
            found_tag = True

    if found_tag:
        if DEBUG:
            print('Calculated version: ' + str(version_loose))
        return str(version_loose)

    return None

def get_next_minor(prerelease_marker):
    """
    get_next_minor does the following:
        - Inspect the branches that fit the convention for a release branch.
        - Choose the latest and increment the minor version.
        - Append .0 to form the new version (e.g., releases/v3.6 becomes 3.7.0)
        - Append a pre-release marker. (e.g. 3.7.0 becomes 3.7.0-20220201+gitf6e6a7025d)
    """

    version_loose = LooseVersion('0.0.0')

    version_new = {}
    # Use refs (not branches) to get local branches plus remote branches
    refs = check_output(['git', 'show-ref']).splitlines()
    for ref in refs:
        release_branch_match = RELEASE_BRANCH_RE.match(ref.split()[1])
        if release_branch_match:
            # Construct a candidate version from this branch name
            version_new['major'] = int(release_branch_match.group('vermaj'))
            version_new['minor'] = int(release_branch_match.group('vermin')) + 1
            version_new['patch'] = 0
            version_new['prerelease'] = prerelease_marker
            new_version_loose = LooseVersion(str(version_new['major']) + '.' +
                                             str(version_new['minor']) + '.' +
                                             str(version_new['patch']) + '-' +
                                             version_new['prerelease'])
            if new_version_loose > version_loose:
                version_loose = new_version_loose
                if DEBUG:
                    print('Found new best version "' + str(version_loose) \
                            + '" based on branch "' \
                            + release_branch_match.group('brname') + '"')
    return str(version_loose)

def get_branch_tags(active_branch_name):
    """
    Returns the tag or tags (as a single string with newlines between tags)
    corresponding to the current branch, which must not be master.  If the
    specified branch is a release branch then return all tags based on the
    major/minor X.Y release version.  If the specified branch is neither master
    nor a release branch, then walk backwards in history until the first tag
    matching the glob 'r*' and return that tag.
    """

    if active_branch_name == 'master':
        raise Exception('this method is not meant to be called while on "master"')
    tags = ''
    release_branch_match = RELEASE_BRANCH_RE.match(active_branch_name)
    if release_branch_match:
        # This is a release branch, so look for tags only on this branch
        tag_glob = 'r' + release_branch_match.group('vermaj') + '.' \
                + release_branch_match.group('vermin') + '.*'
        tags = check_output(['git', 'tag', '--list', tag_glob])
    else:
        # Not a release branch, so look for the most recent tag in history
        commits = check_output(['git', 'log', '--pretty=format:%H',
                                '--no-merges'])
        if len(commits) > 0:
            for commit in commits.splitlines():
                tags = check_output(['git', 'tag', '--points-at',
                                     commit, '--list', 'r*'])
                if len(tags) > 0:
                    # found a tag, we should be done
                    break

    return tags

def process_and_sort_tags(tags):
    """
    Given a string (as returned from get_branch_tags), return a sorted list of
    zero or more tags (sorted based on the LooseVersion comparison) which meet
    the following criteria:
        - a final release tag (i.e., r3.x.y without any pre-release suffix)
        - a pre-release tag which is not superseded by a release tag (i.e.,
          r3.x.y-preX iff r3.x.y does not already exist)
    """

    processed_and_sorted_tags = []
    if not tags or len(tags) == 0:
        return processed_and_sorted_tags

    raw_tags = tags.splitlines()
    # find all the final release tags
    for tag in raw_tags:
        release_tag_match = RELEASE_TAG_RE.match(tag)
        if release_tag_match and not release_tag_match.group('verpre'):
            processed_and_sorted_tags.append(tag)
    # collect together final release tags and pre-release tags for
    # versions that have not yet had a final release
    for tag in raw_tags:
        tag_parts = tag.split('-')
        if len(tag_parts) >= 2 and tag_parts[0] not in processed_and_sorted_tags:
            processed_and_sorted_tags.append(tag)
    processed_and_sorted_tags.sort(key=LooseVersion)

    return processed_and_sorted_tags

def main():
    """
    The algorithm is roughly:

        - Is the --next-minor flag passed? If "yes", then return the next minor
           release with a pre-release marker.
        - Is the current HEAD associated with a tag that looks like a release
           version?
        - If "yes" then use that as the version
        - If "no" then is the current branch master?
        - If "yes" the current branch is master, then return the next minor
           release with a pre-release marker.
        - If "no" the current branch is not master, then determine the most
           recent tag in history; strip any pre-release marker, increment the
           patch version, and append a new pre-release marker
    """

    version_loose = LooseVersion('0.0.0')
    head_commit_short = check_output(['git', 'rev-parse', '--revs-only',
                                      '--short=10', 'HEAD^{commit}']).strip()
    prerelease_marker = datetime.date.today().strftime('%Y%m%d') \
            + '+git' + head_commit_short

    if NEXT_MINOR:
        if DEBUG:
            print('Calculating next minor release')
        return get_next_minor(prerelease_marker)

    head_tag_ver = check_head_tag()
    if head_tag_ver:
        return head_tag_ver

    active_branch_name = check_output(['git', 'rev-parse',
                                       '--abbrev-ref', 'HEAD']).strip()
    if DEBUG:
        print('Calculating release version for branch: ' + active_branch_name)
    if active_branch_name == 'master':
        return get_next_minor(prerelease_marker)

    branch_tags = get_branch_tags(active_branch_name)
    tags = process_and_sort_tags(branch_tags)

    tag = tags[-1] if len(tags) > 0 else ''
    # at this point the RE match is redundant, but convenient for accessing
    # the components of the version string
    release_tag_match = RELEASE_TAG_RE.match(tag)
    if release_tag_match:
        version_new = {}
        version_new['major'] = int(release_tag_match.group('vermaj'))
        version_new['minor'] = int(release_tag_match.group('vermin'))
        version_new['patch'] = int(release_tag_match.group('verpatch')) + 1
        version_new['prerelease'] = prerelease_marker
        new_version_loose = LooseVersion(str(version_new['major']) + '.' +
                                         str(version_new['minor']) + '.' +
                                         str(version_new['patch']) + '-' +
                                         version_new['prerelease'])
        if new_version_loose > version_loose:
            version_loose = new_version_loose
            if DEBUG:
                print('Found new best version "' + str(version_loose) \
                        + '" from tag "' + release_tag_match.group('ver') + '"')

    return str(version_loose)

RELEASE_VER = main()

if DEBUG:
    print('Final calculated release version:')
print(RELEASE_VER)
