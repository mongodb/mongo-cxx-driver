from config_generator.components.funcs.compile import Compile
from config_generator.components.funcs.fetch_det import FetchDET
from config_generator.components.funcs.install_c_driver import InstallCDriver
from config_generator.components.funcs.install_uv import InstallUV
from config_generator.components.funcs.setup import Setup

from config_generator.etc.distros import find_large_distro
from config_generator.etc.function import Function
from config_generator.etc.utils import bash_exec

from shrub.v3.evg_build_variant import BuildVariant
from shrub.v3.evg_command import EvgCommandType, expansions_update
from shrub.v3.evg_task import EvgTask, EvgTaskRef
from shrub.v3.evg_task_group import EvgTaskGroup


TAG = 'atlas-search-indexes'


# pylint: disable=line-too-long
# fmt: off
MATRIX = [
    (7.0),
    (8.0),
]
# fmt: on
# pylint: enable=line-too-long


class TestSearchIndexHelpers(Function):
    name = 'test-search-index-helpers'
    commands = bash_exec(
        command_type=EvgCommandType.TEST,
        working_dir='mongo-cxx-driver',
        script='''\
            export MONGODB_URI=${MONGODB_URI}
            export LD_LIBRARY_PATH=$(pwd)/../mongoc/lib

            ./build/src/mongocxx/test/test_driver "atlas search indexes prose tests"
        '''
    )


def functions():
    return TestSearchIndexHelpers.defn()


def tasks():
    distro_name = 'rhel80'
    distro = find_large_distro(distro_name)

    return [
        EvgTask(
            name=f'{TAG}-{mongodb_version}',
            tags=[TAG, distro_name],
            run_on=distro.name,
            commands=[
                InstallUV.call(),
                InstallCDriver.call(),
                Compile.call(build_type='Debug', vars={'ENABLE_TESTS': 'ON'}),
                TestSearchIndexHelpers.call(),
            ],
        )
        for mongodb_version in MATRIX
    ]


def task_groups():
    return [
        EvgTaskGroup(
            name=f'tg-{TAG}-{mongodb_version}',
            setup_group_can_fail_task=True,
            setup_group_timeout_secs=1800,
            setup_group=[
                Setup.call(),
                FetchDET.call(),
                bash_exec(
                    working_dir='mongo-cxx-driver',
                    env={'MONGODB_VERSION': f'{mongodb_version}'},
                    add_expansions_to_env=True,
                    script='${DRIVERS_TOOLS}/.evergreen/atlas/setup-atlas-cluster.sh',
                ),
                expansions_update(file='mongo-cxx-driver/atlas-expansion.yml'),
            ],
            tasks=[f'{TAG}-{mongodb_version}'],
            teardown_group=[
                bash_exec(
                    working_dir='mongo-cxx-driver',
                    add_expansions_to_env=True,
                    script='${DRIVERS_TOOLS}/.evergreen/atlas/teardown-atlas-cluster.sh'
                )
            ]
        )
        for mongodb_version in MATRIX
    ]


def variants():
    return [
        BuildVariant(
            name=f'{TAG}-matrix',
            display_name=f'{TAG}-matrix',
            tasks=[EvgTaskRef(name=f'tg-{TAG}-{mongodb_version}') for mongodb_version in MATRIX],
        ),
    ]
