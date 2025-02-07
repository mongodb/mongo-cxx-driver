from config_generator.components.funcs.compile import Compile
from config_generator.components.funcs.fetch_c_driver_source import FetchCDriverSource
from config_generator.components.funcs.install_uv import InstallUV
from config_generator.components.funcs.setup import Setup

from config_generator.etc.distros import find_large_distro, make_distro_str
from config_generator.etc.function import Function
from config_generator.etc.utils import bash_exec

from shrub.v3.evg_build_variant import BuildVariant, DisplayTask
from shrub.v3.evg_command import EvgCommandType
from shrub.v3.evg_task import EvgTask, EvgTaskRef

from itertools import product


TAG = 'uninstall-check'


# pylint: disable=line-too-long
# fmt: off
MATRIX = [
    ('rhel80',            'gcc',       ['Debug', 'Release'], ['shared', 'static']),
    ('macos-14-arm64',    'clang',     ['Debug', 'Release'], ['shared', 'static']),
    ('windows-vsCurrent', 'vs2017x64', ['Debug', 'Release'], ['shared', 'static']),
]
# fmt: on
# pylint: enable=line-too-long


class UninstallCheck(Function):
    name = TAG
    commands = bash_exec(
        command_type=EvgCommandType.TEST,
        working_dir='mongo-cxx-driver',
        include_expansions_in_env=['distro_id'],
        script='''\
            case "$OSTYPE" in
            darwin*|linux*) .evergreen/scripts/uninstall_check.sh ;;
            cygwin) cmd.exe /c ".evergreen\\\\scripts\\\\uninstall_check_windows.cmd" ;;
            esac
        '''
    )


def functions():
    return UninstallCheck.defn()


def tasks():
    res = []

    link_type = 'shared'

    for distro_name, compiler, build_types, link_types in MATRIX:
        distro = find_large_distro(distro_name)

        for build_type, link_type in product(build_types, link_types):
            res.append(
                EvgTask(
                    name=f'{TAG}-{make_distro_str(distro_name, compiler, None)}-{build_type.lower()}-{link_type}',
                    tags=[TAG, distro_name, compiler, build_type.lower(), link_type],
                    run_on=distro.name,
                    commands=[
                        Setup.call(),
                        FetchCDriverSource.call(),
                        InstallUV.call(),
                        Compile.call(
                            build_type=build_type,
                            compiler=compiler,
                        ),
                        UninstallCheck.call(),
                    ],
                )
            )

    return res


def variants():
    return [
        BuildVariant(
            name=TAG,
            display_name='Uninstall Check',
            tasks=[EvgTaskRef(name=f'.{TAG}')],
            display_tasks=[
                DisplayTask(
                    name=f'uninstall-check',
                    execution_tasks=[f'.{TAG}'],
                )
            ],
        ),
    ]
