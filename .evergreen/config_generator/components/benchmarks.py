from config_generator.components.funcs.setup import Setup
from config_generator.components.funcs.fetch_c_driver_source import FetchCDriverSource
from config_generator.components.funcs.start_mongod import StartMongod

from config_generator.etc.function import Function, merge_defns
from config_generator.etc.utils import bash_exec

from shrub.v3.evg_build_variant import BuildVariant
from shrub.v3.evg_command import BuiltInCommand, EvgCommandType
from shrub.v3.evg_task import EvgTask, EvgTaskRef


TAG = 'benchmarks'


class RunBenchmarks(Function):
    name = 'benchmarks-run'
    commands = [
        bash_exec(
            command_type=EvgCommandType.SETUP,
            working_dir='mongo-cxx-driver',
            script='etc/microbenchmark-test-data.sh',
        ),
        bash_exec(
            command_type=EvgCommandType.TEST,
            working_dir='mongo-cxx-driver',
            script='build/benchmark/microbenchmarks all',
        ),

        BuiltInCommand(
            command='perf.send',
            type=EvgCommandType.SYSTEM,
            params={
                'name': 'perf',
                'file': 'mongo-cxx-driver/results.json',
            }
        ),
    ]


class CompileBenchmarks(Function):
    name = 'benchmarks-compile'
    commands = bash_exec(
        command_type=EvgCommandType.SETUP,
        working_dir='mongo-cxx-driver',
        script='''\
            set -o errexit
            set -o pipefail
            cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="$(pwd)/../mongoc" -DCMAKE_CXX_STANDARD=20
            cmake --build build --target microbenchmarks --parallel 64
        '''
    )


def functions():
    return merge_defns(
        CompileBenchmarks.defn(),
        RunBenchmarks.defn(),
    )


def tasks():
    distro_name = 'rhel90-dbx-perf'

    return [
        EvgTask(
            name=TAG,
            tags=[TAG, distro_name],
            run_on=f'{distro_name}-large',
            commands=[
                Setup.call(),
                StartMongod.call(mongodb_version='v6.0-perf', topology='single'),
                FetchCDriverSource.call(),
                CompileBenchmarks.call(),
                RunBenchmarks.call(),
            ],
        )
    ]


def variants():
    return [
        BuildVariant(
            name=TAG,
            display_name=TAG,
            tasks=[EvgTaskRef(name=f'.{TAG}')]
        ),
    ]
