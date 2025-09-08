from itertools import product

from shrub.v3.evg_build_variant import BuildVariant
from shrub.v3.evg_command import EvgCommandType, git_get_project, s3_put
from shrub.v3.evg_task import EvgTask, EvgTaskRef
from shrub.v3.evg_task_group import EvgTaskGroup

from config_generator.components.funcs.install_c_driver import InstallCDriver
from config_generator.components.funcs.install_uv import InstallUV
from config_generator.etc.distros import find_large_distro
from config_generator.etc.function import Function, merge_defns
from config_generator.etc.utils import bash_exec

TAG = 'abi-stability'


# pylint: disable=line-too-long
# fmt: off
MATRIX = [
    ('impls',  11),
    ('impls',  17),
    ('stdlib', 17),
    ('stdlib', 20),
    ('stdlib', 23),
]
# fmt: on
# pylint: enable=line-too-long


class AbiComplianceCheck(Function):
    name = 'abi-compliance-check'
    commands = [
        bash_exec(
            command_type=EvgCommandType.SETUP,
            script='mongo-cxx-driver/.evergreen/scripts/abi-compliance-check-setup.sh',
        ),
        bash_exec(
            command_type=EvgCommandType.TEST, script='mongo-cxx-driver/.evergreen/scripts/abi-compliance-check-test.sh'
        ),
        s3_put(
            command_type=EvgCommandType.SYSTEM,
            aws_key='${aws_key}',
            aws_secret='${aws_secret}',
            bucket='mciuploads',
            content_type='text/html',
            display_name='ABI Compliance Check (Stable): ',
            local_files_include_filter='cxx-abi/compat_reports/**/compat_report.html',
            permissions='public-read',
            remote_file='mongo-cxx-driver/${branch_name}/${revision}/${version_id}/${build_id}/${task_id}/${execution}/abi-compliance-check/abi/',
        ),
        s3_put(
            command_type=EvgCommandType.SYSTEM,
            aws_key='${aws_key}',
            aws_secret='${aws_secret}',
            bucket='mciuploads',
            content_type='text/plain',
            display_name='ABI Compliance Check (Stable): ',
            local_files_include_filter='cxx-abi/logs/**/log.txt',
            permissions='public-read',
            remote_file='mongo-cxx-driver/${branch_name}/${revision}/${version_id}/${build_id}/${task_id}/${execution}/abi-compliance-check/abi/',
        ),
        s3_put(
            command_type=EvgCommandType.SYSTEM,
            aws_key='${aws_key}',
            aws_secret='${aws_secret}',
            bucket='mciuploads',
            content_type='text/html',
            display_name='ABI Compliance Check (Unstable): ',
            local_files_include_filter='cxx-noabi/compat_reports/**/compat_report.html',
            permissions='public-read',
            remote_file='mongo-cxx-driver/${branch_name}/${revision}/${version_id}/${build_id}/${task_id}/${execution}/abi-compliance-check/noabi/',
        ),
        s3_put(
            command_type=EvgCommandType.SYSTEM,
            aws_key='${aws_key}',
            aws_secret='${aws_secret}',
            bucket='mciuploads',
            content_type='text/plain',
            display_name='ABI Compliance Check (Unstable): ',
            local_files_include_filter='cxx-noabi/logs/**/log.txt',
            permissions='public-read',
            remote_file='mongo-cxx-driver/${branch_name}/${revision}/${version_id}/${build_id}/${task_id}/${execution}/abi-compliance-check/noabi/',
        ),
    ]


class Abidiff(Function):
    name = 'abidiff'
    commands = [
        bash_exec(
            command_type=EvgCommandType.SETUP,
            script='mongo-cxx-driver/.evergreen/scripts/abidiff-setup.sh',
        ),
        bash_exec(
            command_type=EvgCommandType.TEST,
            script='mongo-cxx-driver/.evergreen/scripts/abidiff-test.sh',
        ),
        s3_put(
            command_type=EvgCommandType.SYSTEM,
            aws_key='${aws_key}',
            aws_secret='${aws_secret}',
            bucket='mciuploads',
            content_type='text/plain',
            display_name='abidiff (Stable): ',
            local_files_include_filter='cxx-abi/*.txt',
            permissions='public-read',
            remote_file='mongo-cxx-driver/${branch_name}/${revision}/${version_id}/${build_id}/${task_id}/${execution}/abidiff/abi/',
        ),
        s3_put(
            command_type=EvgCommandType.SYSTEM,
            aws_key='${aws_key}',
            aws_secret='${aws_secret}',
            bucket='mciuploads',
            content_type='text/plain',
            display_name='abidiff (Unstable): ',
            local_files_include_filter='cxx-noabi/*.txt',
            permissions='public-read',
            remote_file='mongo-cxx-driver/${branch_name}/${revision}/${version_id}/${build_id}/${task_id}/${execution}/abidiff/noabi/',
        ),
    ]


class AbiProhibitedSymbols(Function):
    name = 'abi-prohibited-symbols'
    commands = bash_exec(
        command_type=EvgCommandType.TEST,
        script='mongo-cxx-driver/.evergreen/scripts/abi-prohibited-symbols-test.sh',
    )


def functions():
    return merge_defns(
        AbiComplianceCheck.defn(),
        Abidiff.defn(),
        AbiProhibitedSymbols.defn(),
    )


def generate_tasks():
    funcs = [AbiComplianceCheck, Abidiff, AbiProhibitedSymbols]

    tasks = []

    for func, (polyfill, cxx_standard) in product(funcs, MATRIX):
        if func is Abidiff:
            distro_name = 'ubuntu2204'  # Clang 12, libabigail is not available on RHEL distros.
        else:
            distro_name = 'rhel95'  # Clang 19.

        distro = find_large_distro(distro_name)

        tasks.append(
            EvgTask(
                name=f'{func.name}-{polyfill}-cxx{cxx_standard}',
                tags=[TAG, distro_name, func.name, polyfill, f'cxx{cxx_standard}'],
                run_on=distro.name,
                commands=[
                    func.call(
                        vars={
                            'cxx_standard': f'{cxx_standard}',
                            'polyfill': polyfill,
                        }
                    )
                ],
            )
        )

    return tasks


TASKS = generate_tasks()


def tasks():
    return TASKS


def task_groups():
    return [
        EvgTaskGroup(
            name=f'tg-{TAG}-{polyfill}-cxx{cxx_standard}',
            max_hosts=-1,
            setup_task_can_fail_task=True,
            setup_task=[
                git_get_project(directory='mongo-cxx-driver'),
                InstallUV.call(),
                InstallCDriver.call(),
                bash_exec(
                    command_type=EvgCommandType.SETUP,
                    env={
                        'cxx_standard': f'{cxx_standard}',
                        'polyfill': polyfill,
                    },
                    include_expansions_in_env=[
                        'distro_id',
                        'UV_INSTALL_DIR',
                    ],
                    script='mongo-cxx-driver/.evergreen/scripts/abi-stability-setup.sh',
                ),
                s3_put(
                    command_type=EvgCommandType.SETUP,
                    aws_key='${aws_key}',
                    aws_secret='${aws_secret}',
                    bucket='mciuploads',
                    content_type='text/plain',
                    display_name='ABI Stability Setup: ',
                    local_files_include_filter='*.log',
                    permissions='public-read',
                    remote_file='mongo-cxx-driver/${branch_name}/${revision}/${version_id}/${build_id}/${task_id}/${execution}/abi-stability-setup/',
                ),
            ],
            tasks=[task.name for task in TASKS if polyfill in task.name and f'cxx{cxx_standard}' in task.name],
            teardown_task_can_fail_task=True,
            teardown_task=[
                bash_exec(script='rm -rf *'),
            ],
        )
        for polyfill, cxx_standard in MATRIX
    ]


def variants():
    return [
        BuildVariant(
            name='abi-stability',
            display_name='ABI Stability Checks',
            tasks=[EvgTaskRef(name=f'tg-{TAG}-{polyfill}-cxx{cxx_standard}') for polyfill, cxx_standard in MATRIX],
        )
    ]
