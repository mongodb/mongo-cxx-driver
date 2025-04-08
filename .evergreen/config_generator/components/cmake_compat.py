from config_generator.components.funcs.fetch_c_driver_source import FetchCDriverSource
from config_generator.components.funcs.install_c_driver import InstallCDriver
from config_generator.components.funcs.install_uv import InstallUV
from config_generator.components.funcs.setup import Setup

from config_generator.etc.distros import find_small_distro
from config_generator.etc.function import Function
from config_generator.etc.utils import bash_exec

from shrub.v3.evg_build_variant import BuildVariant
from shrub.v3.evg_command import EvgCommandType
from shrub.v3.evg_task import EvgTask, EvgTaskRef

from itertools import product

TAG = 'cmake-compat'


MATRIX = [
    ("min", [3, 15, 0], [11, 17], [False, True], [False, True]),
    ("max-v3", [3, 31, 6], [11, 17], [False, True], [False, True]),
    ("max", [4, 0, 0], [11, 17], [False, True], [False, True]),
]


class CMakeCompat(Function):
    name = TAG
    commands = [
        bash_exec(
            command_type=EvgCommandType.TEST,
            working_dir='mongo-cxx-driver',
            include_expansions_in_env=[
                'CMAKE_MAJOR_VERSION',
                'CMAKE_MINOR_VERSION',
                'CMAKE_PATCH_VERSION',
                'INSTALL_C_DRIVER',
                'cxx_standard',
            ],
            script='.evergreen/scripts/cmake-compat.sh',
        ),
        bash_exec(
            command_type=EvgCommandType.TEST,
            include_expansions_in_env=[
                'CMAKE_MAJOR_VERSION',
                'CMAKE_MINOR_VERSION',
                'CMAKE_PATCH_VERSION',
                'INSTALL_C_DRIVER',
                'cxx_standard',
                'find_c_driver',
            ],
            script='mongo-cxx-driver/.evergreen/scripts/cmake-compat-check.sh',
        ),
    ]


def functions():
    return CMakeCompat.defn()


def tasks():
    distro_name = 'rhel80'
    distro = find_small_distro(distro_name)

    for name, version, cxx_standards, c_driver_mode, import_modes in MATRIX:
        for cxx_standard, install_c_driver, find_c_driver in product(cxx_standards, c_driver_mode, import_modes):
            commands = [
                Setup.call(), InstallUV.call()
            ] + ([InstallCDriver.call() if install_c_driver else FetchCDriverSource.call()]) + [
                CMakeCompat.call(
                    vars={
                        'CMAKE_MAJOR_VERSION': version[0],
                        'CMAKE_MINOR_VERSION': version[1],
                        'CMAKE_PATCH_VERSION': version[2],
                        'INSTALL_C_DRIVER': 1 if install_c_driver else 0,
                        'cxx_standard': cxx_standard,
                        'find_c_driver': 1 if find_c_driver else 0,
                    },
                ),
            ]

            if install_c_driver:
                c_mode = 'find-c'
            else:
                c_mode = 'add-c'

            if find_c_driver:
                cxx_mode = 'find-cxx'
            else:
                cxx_mode = 'add-cxx'

            yield EvgTask(
                name=f'{TAG}-{name}-cxx{cxx_standard}-{c_mode}-{cxx_mode}',
                tags=[TAG, f'cmake-{name}', f'cmake-{c_mode}', f'cxx{cxx_standard}', f'cmake-{cxx_mode}'],
                run_on=distro.name,
                commands=commands,
            )


def variants():
    return [
        BuildVariant(
            name=f'{TAG}-matrix',
            display_name='CMake Compatibility Matrix',
            tasks=[EvgTaskRef(name=f'.{TAG}')],
        ),
    ]
