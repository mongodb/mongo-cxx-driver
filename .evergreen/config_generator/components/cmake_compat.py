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

# pylint: disable=line-too-long
# fmt: off
MATRIX = [
    ("min",    [3, 15, 4], [11, 17]),
    ("max-v3", [3, 31, 7], [11, 17]),
    ("max",    [4,  0, 1], [11, 17]),
]
# fmt: on


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
            ],
            script='mongo-cxx-driver/.evergreen/scripts/cmake-compat-check.sh',
        ),
    ]


def functions():
    return CMakeCompat.defn()


def tasks():
    distro_name = 'rhel80'
    distro = find_small_distro(distro_name)

    # Test importing C Driver libraries via both add_subdirectory() and find_package().
    install_c_driver_modes = [False, True]

    for name, version, cxx_standards in MATRIX:
        for cxx_standard, install_c_driver in product(cxx_standards, install_c_driver_modes):
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
                    },
                ),
            ]

            if install_c_driver:
                c_mode = 'find-c'
            else:
                c_mode = 'add-c'

            yield EvgTask(
                name=f'{TAG}-{name}-cxx{cxx_standard}-{c_mode}',
                tags=[TAG, f'cmake-{name}', f'cxx{cxx_standard}', f'cmake-{c_mode}'],
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
