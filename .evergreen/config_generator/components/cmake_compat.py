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

TAG = 'cmake-compat'

# pylint: disable=line-too-long
# fmt: off
MATRIX = [
    # As-if `cmake~=<version>` (PEP 0440).
    ("min",    "3.15.0"),
    ("max-v3", "3.0"   ),
    ("max",    "4.0.0" ),
]
# fmt: on


class CMakeCompat(Function):
    name = TAG
    commands = [
        bash_exec(
            command_type=EvgCommandType.TEST,
            working_dir='mongo-cxx-driver',
            include_expansions_in_env=[
                'CMAKE_VERSION',
                'distro_id',
                'INSTALL_C_DRIVER',
                'UV_INSTALL_DIR',
            ],
            script='.evergreen/scripts/cmake-compat.sh',
        ),
        bash_exec(
            command_type=EvgCommandType.TEST,
            include_expansions_in_env=[
                'CMAKE_VERSION',
                'distro_id',
                'INSTALL_C_DRIVER',
                'UV_INSTALL_DIR',
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

    for name, version in MATRIX:
        for install_c_driver in install_c_driver_modes:
            commands = [
                Setup.call(),
                InstallUV.call(),
                (InstallCDriver.call() if install_c_driver else FetchCDriverSource.call()),
                CMakeCompat.call(
                    vars={
                        'CMAKE_VERSION': version,
                        'INSTALL_C_DRIVER': int(install_c_driver),
                    },
                ),
            ]

            if install_c_driver:
                c_mode = 'find-c'
            else:
                c_mode = 'add-c'

            yield EvgTask(
                name=f'{TAG}-{name}-{c_mode}',
                tags=[TAG, f'cmake-{name}', f'cmake-{c_mode}'],
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
