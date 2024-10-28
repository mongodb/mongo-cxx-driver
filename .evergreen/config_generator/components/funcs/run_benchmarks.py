from shrub.v3.evg_command import BuiltInCommand, EvgCommandType

from config_generator.etc.function import Function
from config_generator.etc.utils import bash_exec


class RunBenchmarks(Function):
    name = 'run benchmarks'
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

    @classmethod
    def call(cls, **kwargs):
        return cls.default_call(**kwargs)


def functions():
    return RunBenchmarks.defn()
