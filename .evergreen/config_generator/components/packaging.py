from config_generator.components.funcs.setup import Setup

from config_generator.etc.distros import find_large_distro
from config_generator.etc.function import Function, merge_defns
from config_generator.etc.utils import Task, bash_exec

from shrub.v3.evg_build_variant import BuildVariant
from shrub.v3.evg_command import EvgCommandType, s3_put
from shrub.v3.evg_task import EvgTaskRef


TAG = 'packaging'


class DebianPackageBuild(Function):
    name = 'build-package-debian'
    desc = 'debian'
    commands = [
        bash_exec(
            command_type=EvgCommandType.TEST,
            working_dir='mongo-cxx-driver',
            script='''\
                set -o errexit
                export IS_PATCH="${is_patch}"
                .evergreen/scripts/debian_package_build.sh
            ''',
        ),
        s3_put(
            aws_key='${aws_key}',
            aws_secret='${aws_secret}',
            bucket='mciuploads',
            content_type='${content_type|application/x-gzip}',
            display_name='"deb.tar.gz"',
            local_file='deb.tar.gz',
            permissions='public-read',
            remote_file='mongo-cxx-driver/${branch_name}/${revision}/${version_id}/${build_id}/${execution}/debian-packages.tar.gz',
        ),
    ]


class RpmPackageBuild(Function):
    name = 'build-package-rpm'
    desc = 'rpm'
    commands = [
        bash_exec(
            command_type=EvgCommandType.TEST,
            working_dir='mongo-cxx-driver',
            script='.evergreen/scripts/build_snapshot_rpm.sh',
        ),
        s3_put(
            aws_key='${aws_key}',
            aws_secret='${aws_secret}',
            bucket='mciuploads',
            content_type='${content_type|application/x-gzip}',
            local_file='rpm.tar.gz',
            permissions='public-read',
            remote_file='mongo-cxx-driver/${branch_name}/${revision}/${version_id}/${build_id}/${execution}/rpm-packages.tar.gz',
        ),
    ]


# pylint: disable=line-too-long
# fmt: off
MATRIX = [
    (DebianPackageBuild,        'debian12-latest'),  # Debian packaging.
    (RpmPackageBuild,           'rhel92-arm64'   ),  # RHEL packaging.
]
# fmt: on
# pylint: enable=line-too-long


def functions():
    return merge_defns(
        DebianPackageBuild.defn(),
        RpmPackageBuild.defn(),
    )


def tasks():
    return [
        Task(
            name=f'{TAG}-{fn.desc}',
            tags=[TAG, distro_name],
            run_on=find_large_distro(distro_name).name,
            allowed_requesters=[
                'ad_hoc',
                'commit',
                # 'github_merge_queue'
                # 'github_pr',
                # 'github_tag',
                'patch',
                'trigger',
            ],
            commands=[
                Setup.call(),
                fn.call(),
            ]
        )
        for fn, distro_name in MATRIX
    ]


def variants():
    return [
        BuildVariant(
            name=TAG,
            display_name='Linux Distro Packaging',
            tasks=[EvgTaskRef(name=f'.{TAG}')],
        ),
    ]
