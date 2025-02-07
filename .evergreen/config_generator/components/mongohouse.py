from config_generator.components.funcs.compile import Compile
from config_generator.components.funcs.fetch_c_driver_source import FetchCDriverSource
from config_generator.components.funcs.install_uv import InstallUV
from config_generator.components.funcs.setup import Setup

from config_generator.etc.distros import find_large_distro
from config_generator.etc.function import Function, merge_defns
from config_generator.etc.utils import bash_exec

from shrub.v3.evg_build_variant import BuildVariant
from shrub.v3.evg_command import EvgCommandType
from shrub.v3.evg_task import EvgTask, EvgTaskRef


TAG = 'mongohouse'


class BuildMongohouse(Function):
    name = 'build_mongohouse'
    commands = bash_exec(
        command_type=EvgCommandType.SETUP,
        script='''\
            if [ ! -d "drivers-evergreen-tools" ]; then
                git clone https://github.com/mongodb-labs/drivers-evergreen-tools.git
            fi
            cd drivers-evergreen-tools
            export DRIVERS_TOOLS=$(pwd)

            .evergreen/atlas_data_lake/pull-mongohouse-image.sh
        '''
    )


class RunMongohouse(Function):
    name = 'run_mongohouse'
    commands = bash_exec(
        command_type=EvgCommandType.SETUP,
        script='''\
            cd drivers-evergreen-tools
            export DRIVERS_TOOLS=$(pwd)

            .evergreen/atlas_data_lake/run-mongohouse-image.sh
        '''
    )


class TestMongohouse(Function):
    name = 'test_mongohouse'
    commands = bash_exec(
        command_type=EvgCommandType.TEST,
        working_dir='mongo-cxx-driver',
        include_expansions_in_env=['distro_id'],
        script='.evergreen/scripts/test-mongohouse.sh'
    )


def functions():
    return merge_defns(
        BuildMongohouse.defn(),
        RunMongohouse.defn(),
        TestMongohouse.defn(),
    )


def tasks():
    distro_name = 'ubuntu2204'
    distro = find_large_distro(distro_name)

    return [
        EvgTask(
            name='test_mongohouse',
            tags=[TAG, distro_name],
            run_on=distro.name,
            commands=[
                Setup.call(),
                FetchCDriverSource.call(),
                InstallUV.call(),
                Compile.call(build_type='Release', vars={'ENABLE_TESTS': 'ON'}),
                BuildMongohouse.call(),
                RunMongohouse.call(),
                TestMongohouse.call(),
            ],
        ),
    ]


def variants():
    return [
        BuildVariant(
            name='mongohouse',
            display_name='Mongohouse',
            tasks=[EvgTaskRef(name=f'.{TAG}')]
        ),
    ]
