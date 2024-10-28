from config_generator.components.funcs.install_c_driver import InstallCDriver

from config_generator.etc.distros import find_large_distro
from config_generator.etc.function import Function, merge_defns
from config_generator.etc.utils import TaskGroup, bash_exec

from shrub.v3.evg_command import EvgCommandType, git_get_project, s3_put
from shrub.v3.evg_task import EvgTask, EvgTaskRef
from shrub.v3.evg_build_variant import BuildVariant, DisplayTask


TAG = 'abi-stability'


# pylint: disable=line-too-long
# fmt: off
MATRIX = [
    ('polyfill', 11),
    ('stdlib',   17),
]
# fmt: on
# pylint: enable=line-too-long


class AbiComplianceCheck(Function):
    name = 'abi-compliance-check'
    commands = [
        bash_exec(
            command_type=EvgCommandType.SETUP,
            script='mongo-cxx-driver/.evergreen/scripts/abi-compliance-check-setup.sh'
        ),
        bash_exec(
            command_type=EvgCommandType.TEST,
            script='mongo-cxx-driver/.evergreen/scripts/abi-compliance-check-test.sh'
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
            remote_file='mongo-cxx-driver/${branch_name}/${revision}/${version_id}/${build_id}/${execution}/abi-compliance-check/abi/',
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
            remote_file='mongo-cxx-driver/${branch_name}/${revision}/${version_id}/${build_id}/${execution}/abi-compliance-check/abi/',
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
            remote_file='mongo-cxx-driver/${branch_name}/${revision}/${version_id}/${build_id}/${execution}/abi-compliance-check/noabi/',
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
            remote_file='mongo-cxx-driver/${branch_name}/${revision}/${version_id}/${build_id}/${execution}/abi-compliance-check/noabi/',
        ),
    ]

    @classmethod
    def call(cls, **kwargs):
        return cls.default_call(**kwargs)


class Abidiff(Function):
    name = 'abidiff'
    commands = [
        bash_exec(
            command_type=EvgCommandType.SETUP,
            script='mongo-cxx-driver/.evergreen/scripts/abidiff-setup.sh'
        ),
        bash_exec(
            command_type=EvgCommandType.TEST,
            script='mongo-cxx-driver/.evergreen/scripts/abidiff-test.sh'
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
            remote_file='mongo-cxx-driver/${branch_name}/${revision}/${version_id}/${build_id}/${execution}/abidiff/abi/',
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
            remote_file='mongo-cxx-driver/${branch_name}/${revision}/${version_id}/${build_id}/${execution}/abidiff/noabi/',
        ),
    ]

    @classmethod
    def call(cls, **kwargs):
        return cls.default_call(**kwargs)


class AbiProhibitedSymbols(Function):
    name = 'abi-prohibited-symbols'
    commands = bash_exec(
        command_type=EvgCommandType.TEST,
        script='mongo-cxx-driver/.evergreen/scripts/abi-prohibited-symbols-test.sh'
    )

    @classmethod
    def call(cls, **kwargs):
        return cls.default_call(**kwargs)


def functions():
    return merge_defns(
        AbiComplianceCheck.defn(),
        Abidiff.defn(),
        AbiProhibitedSymbols.defn(),
    )


def tasks():
    distro_name = 'ubuntu2204'
    distro = find_large_distro(distro_name)

    return [
        EvgTask(
            name=func.name,
            tags=[TAG, func.name, distro_name],
            run_on=distro.name,
            commands=[func.call()],
        )
        for func in [AbiComplianceCheck, Abidiff, AbiProhibitedSymbols]
    ]


def task_groups():
    return [
        TaskGroup(
            name=f'tg-{TAG}',
            max_hosts=-1,
            setup_group_can_fail_task=True,
            setup_task=[
                git_get_project(directory='mongo-cxx-driver'),
                InstallCDriver.call(),
                bash_exec(
                    include_expansions_in_env=['cxx_standard'],
                    script='mongo-cxx-driver/.evergreen/scripts/abi-stability-setup.sh'
                ),
            ],
            tasks=[f'.{TAG}'],
            teardown_task_can_fail_task=True,
            teardown_task=[bash_exec(script='rm -rf *'),],
        )
    ]


def variants():
    return [
        BuildVariant(
            name=f'abi-stability-{name}',
            display_name=f'ABI Stability Checks ({name})',
            expansions={
                'cxx_standard': f'{cxx_standard}',  # Use a polyfill library.
            },
            tasks=[EvgTaskRef(name='tg-abi-stability')],
            display_tasks=[
                DisplayTask(
                    name=f'ABI Stability Checks ({name})',
                    execution_tasks=[f'.{TAG}'],
                )
            ],
        )
        for name, cxx_standard in MATRIX
    ]
