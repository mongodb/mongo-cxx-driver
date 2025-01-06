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
    ('debian10',          'gcc',       [         'Release'], ['shared']),
    ('debian11',          'gcc',       [         'Release'], ['shared']),
    ('debian12',          'gcc',       [         'Release'], ['shared']),
    ('windows-64-vs2015', 'vs2015x64', ['Debug', 'Release'], ['shared']),
    ('ubuntu1804',        'gcc',       [         'Release'], ['shared']),
    ('ubuntu2004',        'gcc',       [         'Release'], ['shared']),
]
# fmt: on
# pylint: enable=line-too-long


class UninstallCheck(Function):
    name = TAG
    commands = bash_exec(
        command_type=EvgCommandType.TEST,
        working_dir='mongo-cxx-driver',
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
