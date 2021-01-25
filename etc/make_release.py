#!/usr/bin/env python3

#
# Copyright 2020 MongoDB, Inc.
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
Make a release of the C++ Driver, including steps associated with the CXX
project in Jira, and with the mongodb/mongo-cxx-driver GitHub repository.
See releasing.md for complete release instructions.
"""

# CXX Project ID - 11980
#
# ------------------------
# | Issue Type  |   ID   |
# ------------------------
# | Backport    | 10300  |
# | Bug         | 1      |
# | Epic        | 11     |
# | Improvement | 4      |
# | New Feature | 2      |
# | Story       | 12     |
# | Task        | 3      |
# ------------------------

import re
from distutils.version import LooseVersion
import os
import subprocess
import sys
import tempfile

import click # pip install Click
from git import Repo # pip install GitPython
from github import Github # pip install PyGithub
from jira import JIRA # pip install jira

if sys.version_info < (3, 0, 0):
    raise RuntimeError("This script requires Python 3 or higher")

RELEASE_TAG_RE = re.compile('r(?P<ver>(?P<vermaj>[0-9]+)\\.(?P<vermin>[0-9]+)'
                            '\\.(?P<verpatch>[0-9]+)(?:-(?P<verpre>.*))?)')
CXX_PROJ_ID = 11980
ISSUE_TYPE_ID = {'Backport': '10300',
                 'Bug': '1',
                 'Epic': '11',
                 'Improvement': '4',
                 'New Feature': '2',
                 'Story': '12',
                 'Task': '3'
                }

@click.command()
@click.option('--jira-creds-file',
              '-j',
              default='jira_creds.txt',
              show_default=True,
              help='Jira OAuth credentials file')
@click.option('--github-token-file',
              '-g',
              default='github_token.txt',
              show_default=True,
              help='GitHub token file')
@click.option('--allow-open-issues',
              '-a',
              is_flag=True,
              help='Allow open issues; NOT RECOMMENDED')
@click.option('--remote',
              '-r',
              default='origin',
              show_default=True,
              help='The remote reference which points to the mongodb/mongo-cxx-driver repo')
@click.option('--c-driver-install-dir',
              default=os.getcwd() + '/../mongoc',
              show_default=True,
              help='When building the C driver, install to this directory')
@click.option('--c-driver-build-ref',
              default='master',
              show_default=True,
              help='When building the C driver, build at this Git reference')
@click.option('--with-c-driver',
              help='Instead of building the C driver, use the one installed at this path')
@click.option('--dist-file',
              help='Don\'t build anything; use this C++ driver distribution tarball')
@click.option('--skip-distcheck',
              is_flag=True,
              help="Only build the distribution tarball (do not build the driver or run tests)")
@click.option('--output-file',
              '-o',
              help='Send release announcement draft output to the specified file')
@click.option('--dry-run',
              '-n',
              is_flag=True,
              help='Do not perform any actions which modify remote resources')
@click.option('--quiet',
              '-q',
              is_flag=True,
              help='Produce fewer progress messages')
@click.argument('git-revision', required=True)
# pylint: disable=too-many-arguments,too-many-locals,too-many-branches,too-many-statements
def release(jira_creds_file,
            github_token_file,
            allow_open_issues,
            remote,
            c_driver_install_dir,
            c_driver_build_ref,
            with_c_driver,
            dist_file,
            skip_distcheck,
            output_file,
            git_revision,
            dry_run,
            quiet):
    """
    Perform the steps associated with the release.
    """

    # Read Jira credentials and GitHub token first, to check that
    # user has proper credentials before embarking on lengthy builds.
    jira_options = {'server': 'https://jira.mongodb.org'}
    jira_oauth_dict = read_jira_oauth_creds(jira_creds_file)
    auth_jira = JIRA(jira_options, oauth=jira_oauth_dict)

    github_token = read_github_creds(github_token_file)
    auth_gh = Github(github_token)

    if not is_valid_remote(remote):
        click.echo('The the remote "{}" does not point to the mongodb/mongo-cxx-driver '
                   'repo...exiting!'.format(remote), err=True)
        sys.exit(1)

    if dry_run:
        click.echo('DRY RUN! No remote modifications will be made!')
    if not quiet:
        print_banner(git_revision)

    release_tag, release_version = get_release_tag(git_revision)

    if not release_tag:
        click.echo('No release tag points to {}'.format(git_revision), err=True)
        click.echo('Nothing to do here...exiting!', err=True)
        sys.exit(1)

    if not working_dir_on_valid_branch(release_version):
        # working_dir_on_valid_branch() has already produced an error message
        sys.exit(1)

    if not release_tag_points_to_head(release_tag):
        click.echo('Tag {} does not point to HEAD...exiting!'.format(release_tag), err=True)
        sys.exit(1)

    is_pre_release = check_pre_release(release_tag)

    if not quiet:
        found_msg = 'Found release tag {}'.format(release_tag)
        if is_pre_release:
            found_msg = 'Found pre-release tag {}'.format(release_tag)
        click.echo(found_msg)

    if dist_file:
        if not os.path.exists(dist_file):
            click.echo('Specified distribution tarball does not exist...exiting!', err=True)
            sys.exit(1)
    else:
        c_driver_dir = ensure_c_driver(c_driver_install_dir, c_driver_build_ref,
                                       with_c_driver, quiet)
        if not c_driver_dir:
            click.echo('C driver not built or not found...exiting!', err=True)
            sys.exit(1)

        dist_file = build_distribution(release_tag, release_version, c_driver_dir, quiet,
                                       skip_distcheck)
        if not dist_file:
            click.echo('C++ driver distribution not built or not found...exiting!', err=True)
            sys.exit(1)

    jira_vers_dict = get_jira_project_versions(auth_jira)

    if release_version not in jira_vers_dict.keys():
        click.echo('Version "{}" not found in Jira.  Cannot release!'
                   .format(release_version), err=True)
        sys.exit(1)
    if jira_vers_dict[release_version].released:
        click.echo('Version "{}" already released in Jira.  Cannot release again!'
                   .format(release_version), err=True)
        sys.exit(1)

    issues = get_all_issues_for_version(auth_jira, release_version)

    if not allow_open_issues and not all_issues_closed(issues):
        # all_issues_closed() has already produced an error message
        sys.exit(1)

    release_notes_text = generate_release_notes(issues, release_version)

    gh_repo = auth_gh.get_repo('mongodb/mongo-cxx-driver')
    gh_release_dict = get_github_releases(gh_repo)

    if release_tag in gh_release_dict.keys():
        click.echo('Version "{}" already released in GitHub.  Cannot release again!'
                   .format(release_tag), err=True)
        sys.exit(1)

    if dry_run:
        click.echo('DRY RUN!  Not creating release for tag "{}"'.format(release_tag))

        if output_file:
            click.echo('Release notes draft is here: {}'.format(output_file))

            with open(output_file, 'w') as output_stream:
                output_stream.write(release_notes_text)
    else:
        create_github_release_draft(gh_repo, release_tag, is_pre_release, dist_file,
                                    release_notes_text, output_file, quiet)

# pylint: enable=too-many-arguments,too-many-locals,too-many-branches,too-many-statements
def is_valid_remote(remote):
    """
    Determines whether the specified remote of the current working directory is
    associated with the "mongodb/mongo-cxx-driver" GitHub project.  The
    comparison is made against the first URL of the "origin" remote.
    """

    remote_re = re.compile('^(https://github.com/|git@github.com:)'
                           'mongodb/mongo-cxx-driver(\\.git)?$')
    repo = Repo('.')

    return bool(remote_re.match(list(repo.remote(remote).urls)[0]))

def print_banner(git_revision):
    """
    Print a nice looking banner.
    """

    banner = "**************************************************************\n"
    banner += "*********** C R E A T I N G ***** R E L E A S E **************\n"
    banner += "**************************************************************\n"
    banner += "This build is for Git revision {}".format(git_revision)

    click.echo(banner)

def get_release_tag(git_revision):
    """
    Find the "best" release tag associated with the given commit.  For this
    purpose, "best" is defined as highest version when using standard versioning
    semantics to compare between versions.  If there is only a single release
    tag associated with the commit, then that one is returned.  If no release
    tags are associated with the commit, then None is returned.
    """

    repo = Repo('.')
    commit_to_release = repo.commit(git_revision)
    release_tags = []
    for tag in repo.tags:
        if tag.commit == commit_to_release and RELEASE_TAG_RE.match(tag.name):
            release_tags.append(tag)

    best_tag = None
    if release_tags:
        version_loose = LooseVersion('0.0.0')
        for tag in release_tags:
            tag_ver = LooseVersion(RELEASE_TAG_RE.match(tag.name).group('ver'))
            if tag_ver > version_loose:
                version_loose = tag_ver
                best_tag = tag

    return (best_tag.name, version_loose.vstring) if best_tag else (None, None)

def working_dir_on_valid_branch(release_version):
    """
    Checks that the current branch in the working directory matches the expected
    value based on the value in the third position:
    '0' in the third position -> expected branch: master
    anything other than '0' in the third position -> expected branch: releases/vX.Y
    """

    version_loose = LooseVersion(release_version)
    repo = Repo('.')

    if version_loose.version[2] == 0:
        exp_branch = 'master'
    else:
        exp_branch = 'releases/v{}.{}'.format(version_loose.version[0], version_loose.version[1])

    if repo.active_branch.name == exp_branch:
        return True

    click.echo('Expected branch "{}" for release version "{}", but working '
               'directory is on branch "{}"...exiting!'
               .format(exp_branch, release_version, repo.active_branch.name), err=True)
    return False

def release_tag_points_to_head(release_tag):
    """
    Checks whether the given tag to determine if it points to the HEAD commit of
    the current working directory.
    """

    repo = Repo('.')
    tag_commit = repo.commit(release_tag)

    return tag_commit == repo.head.commit

def check_pre_release(tag_name):
    """
    Check the given tag to determine if it is a release tag, that is, whether it
    is of the form rX.Y.Z.  Tags that do not match (e.g., because they are
    suffixed with someting like -beta# or -rc#) are considered pre-release tags.
    Note that this assumes that the tag name has been validated to ensure that
    it starts with something like rX.Y.Z and nothing else.
    """

    release_re = re.compile('^r[0-9]+\\.[0-9]+\\.[0-9]+')

    return not bool(release_re.match(tag_name))

def ensure_c_driver(c_driver_install_dir, c_driver_build_ref, with_c_driver, quiet):
    """
    Ensures that there is a properly installed C driver, returning the location
    of the C driver installation.  If the with_c_driver parameter is set and
    points to a proper installation of the C driver, then this function simply
    returns that directory.  Otherwise, delegates to another function to build
    the C driver and install it to the directory specified by the
    c_driver_install_dir parameter.
    """

    if with_c_driver:
        bson_h = os.path.join(with_c_driver, 'include/libbson-1.0/bson/bson.h')
        mongoc_h = os.path.join(with_c_driver, 'include/libmongoc-1.0/mongoc/mongoc.h')
        if os.path.exists(bson_h) and os.path.exists(mongoc_h):
            return with_c_driver
        if not quiet:
            click.echo('A required component of the C driver is missing!', err=True)
        return None

    return build_c_driver(c_driver_install_dir, c_driver_build_ref, quiet)

def build_c_driver(c_driver_install_dir, c_driver_build_ref, quiet):
    """
    Build the C driver and install to the specified directory.  If the build is
    successful, then return the directory where the C driver was installed,
    otherwise return None.
    """

    mongoc_prefix = os.path.abspath(c_driver_install_dir)

    if not quiet:
        click.echo(f'Building C Driver at {mongoc_prefix} (this could take several minutes)')
        click.echo('Pass --with-c-driver to use an existing installation')

    env = os.environ
    env['PREFIX'] = mongoc_prefix
    if not c_driver_build_ref:
        c_driver_build_ref = 'master'
    run_shell_script('./.evergreen/install_c_driver.sh ' + c_driver_build_ref, env=env)

    if not quiet:
        click.echo('C Driver build was successful.')
        click.echo('Version "{}" was installed to "{}".'
                   .format(c_driver_build_ref, mongoc_prefix))
    return mongoc_prefix

def build_distribution(release_tag, release_version, c_driver_dir, quiet, skip_distcheck):
    """
    Perform the necessary steps to build the distribution tarball which will be
    attached to the release in GitHub.  Return the path to the distribution
    tarball for a successful build and return None for a failed build.
    """

    dist_file = 'build/mongo-cxx-driver-{}.tar.gz'.format(release_tag)

    if not quiet:
        click.echo('Building C++ distribution tarball: {}'.format(dist_file))

    if os.path.exists(dist_file):
        click.echo('Distribution tarball already exists: {}'.format(dist_file))
        click.echo('Refusing to build distribution tarball.')
        click.echo('To use the existing tarball, pass: --dist-file {}'.format(dist_file), err=True)
        return None

    if os.path.exists('build/CMakeCache.txt'):
        click.echo('Remnants of prior build found in ./build directory.')
        click.echo('Refusing to build distribution tarball.')
        click.echo('Clear ./build with "git clean -xdf ./build"', err=True)
        return None

    run_shell_script('. .evergreen/find_cmake.sh;'
                     'cd build;'
                     'echo ' + release_version + ' > VERSION_CURRENT;'
                     '${CMAKE} -DCMAKE_BUILD_TYPE=Release '
                     '-DCMAKE_PREFIX_PATH="' + c_driver_dir + '" '
                     '-DENABLE_UNINSTALL=ON ..;'
                     'cmake --build . --target dist')

    if not quiet:
        click.echo('C++ Driver build was successful.')
        click.echo('Distribution file: {}'.format(dist_file))

    if not skip_distcheck:
        click.echo('Building C++ driver from tarball and running tests.')
        click.echo('This may take several minutes. This may be skipped with --skip_distcheck')
        run_shell_script('cmake --build build --target distcheck')
    return dist_file

def read_jira_oauth_creds(jira_creds_file):
    """
    Read the Jira Oauth credentials file and return a dictionary containing the
    credentials.
    """

    creds_re = re.compile(
        r'^[\s\S]*access_token\s*:\s*(\S+)\s*'
        r'access_token_secret\s*:\s*(\S+)\s*'
        r'consumer_key\s*:\s*(\S+)\s*'
        r'key_cert\s*:\s*(-.*-)[\s\S]*$')

    oauth_dict = {}
    with open(jira_creds_file, 'rb') as creds_stream:
        creds_data = creds_stream.read().decode('ascii')
        creds_match = creds_re.match(creds_data)
        if creds_match:
            oauth_dict['access_token'] = creds_match.group(1)
            oauth_dict['access_token_secret'] = creds_match.group(2)
            oauth_dict['consumer_key'] = creds_match.group(3)
            # Fix the double-backslash created by the decode() call above
            oauth_dict['key_cert'] = creds_match.group(4).replace("\\n", "\n")

    return oauth_dict

def get_jira_project_versions(auth_jira):
    """
    Return a dictionary of release versions available in the Jira project.
    """

    jira_proj = auth_jira.project(CXX_PROJ_ID)
    jira_vers = auth_jira.project_versions(jira_proj)
    # Make the list of versions into an easier to use dictionary
    # (keyed by release version name)
    return dict(zip([v.name for v in jira_vers], jira_vers))

def get_all_issues_for_version(auth_jira, release_version):
    """
    Return a list of all issues in the project assigned to the given release.
    """

    jql_query = 'project={} and fixVersion={} ORDER BY issueKey ASC'\
            .format(str(CXX_PROJ_ID), release_version)
    return auth_jira.search_issues(jql_query, maxResults=0)

def all_issues_closed(issues):
    """
    Check to ensure that all issues are 'Closed'.  Produce appropriate error
    message(s) and return False if any open issues are found.
    """

    status_set = set(i.fields.status.name for i in issues)

    if status_set.difference({'Closed'}):
        msg = 'Open tickets found.  Cannot release!'
        msg += '\nThe following open tickets were found:'
        click.echo(msg, err=True)
        open_filter = lambda x: x.fields.status.name != 'Closed'
        open_issues = [i.key for i in filter(open_filter, issues)]
        click.echo('{}'.format(", ".join(open_issues)), err=True)
        return False

    return True

def generate_release_notes(issues, release_version):
    """
    Produce HTML release notes which can be used as part of the project release
    announcement.
    """

    release_notes = '<h1>Release Notes - C++ Driver - Version {}</h1>\n'.format(release_version)
    release_notes += '<h2>Bug</h2>\n'
    release_notes += '<ul>\n'
    bug_filter = lambda i: i.fields.issuetype.id == ISSUE_TYPE_ID['Bug']
    release_notes += print_issues(list(filter(bug_filter, issues)))
    release_notes += '</ul>\n'
    release_notes += '<h2>New Feature</h2>\n'
    release_notes += '<ul>\n'
    new_feature_filter = lambda i: i.fields.issuetype.id == ISSUE_TYPE_ID['New Feature']
    release_notes += print_issues(list(filter(new_feature_filter, issues)))
    release_notes += '</ul>\n'
    release_notes += '<h2>Improvement</h2>\n'
    release_notes += '<ul>\n'
    improvement_filter = lambda i: i.fields.issuetype.id == ISSUE_TYPE_ID['Improvement']
    release_notes += print_issues(list(filter(improvement_filter, issues)))
    release_notes += '</ul>\n'

    return release_notes

def print_issues(issues):
    """
    Append each of the issues to the release notes string in a form suitable for
    HTML output.
    """

    text = ''
    for issue in issues:
        text += '<li>'
        text += '[<a href="{}">{}</a>] - {}'\
                .format(issue.permalink(), issue.key, issue.fields.summary)
        text += '</li>\n'

    return text

def read_github_creds(github_token_file):
    """
    Read the GitHub token from the specified file and return it as a string.
    """

    token_re = re.compile('^(?:Token - )?(?P<tok>[0-9a-f]{40}).*$')
    github_token = None

    with open(github_token_file, 'rb') as token_stream:
        token_data = token_stream.read().decode('ascii')
        token_match = token_re.match(token_data)
        if token_match:
            github_token = token_match.group('tok')
        else:
            click.echo('No Github token found in file "{}"'
                       .format(github_token_file), err=True)

    return github_token

def get_github_releases(gh_repo):
    """
    Return a dictionary of releases which have been made in the GitHub repository.
    """

    gh_releases = gh_repo.get_releases()
    # Make the list of releases into an easier to use dictionary
    # (keyed by release tag name)
    return dict(zip([r.tag_name for r in gh_releases], gh_releases))

# pylint: disable=too-many-arguments
def create_github_release_draft(gh_repo,
                                release_tag,
                                is_pre_release,
                                dist_file,
                                release_notes_text,
                                output_file,
                                quiet):
    """
    Create a GitHub release in DRAFT state and attach release artifacts.
    """

    release_name = 'MongoDB C++11 Driver {}'.format(release_tag)

    if output_file:
        if not quiet:
            click.echo('Draft release announcement is here: {}'.format(output_file))

        with open(output_file, 'w') as output_stream:
            output_stream.write(release_notes_text)

    gh_release = gh_repo.create_git_release(tag=release_tag, name=release_name,
                                            message=release_notes_text, draft=True,
                                            prerelease=is_pre_release)
    gh_release.upload_asset(dist_file)

    click.echo('Github release has been created.  Review and publish here: {}'
               .format(gh_release.html_url))
    click.echo('Mark the version as released in Jira.')
    click.echo('Then generate and publish documentation.')

# pylint: enable=too-many-arguments

def run_shell_script(script, env=None):
    """
    Execute a shell process, and returns contents of stdout. Raise an error on failure.
    """

    proc = subprocess.Popen(script,
                            stdout=subprocess.PIPE, stderr=subprocess.PIPE, env=env,
                            shell=True)
    outs, errs = proc.communicate()
    if proc.returncode == 0:
        return outs

    with tempfile.NamedTemporaryFile(suffix='.out', delete=False) as tmpfp:
        tmpfp.write(outs)
        stdout_path = tmpfp.name

    with tempfile.NamedTemporaryFile(suffix='.err', delete=False) as tmpfp:
        tmpfp.write(errs)
        stderr_path = tmpfp.name

    raise RuntimeError(f'Script failed: {script}\n'
                       'Consult output logs:\n'
                       f'stdout: {stdout_path}\n'
                       f'stderr: {stderr_path}\n')

if __name__ == '__main__':
    release()
