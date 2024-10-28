from shrub.v3.evg_command import EvgCommandType

from config_generator.etc.function import Function
from config_generator.etc.utils import bash_exec


class CompileBenchmarks(Function):
    name = 'compile_benchmarks'
    commands = bash_exec(
        command_type=EvgCommandType.SETUP,
        env={
            'CC': '${cc_compiler}',
            'CXX': '${cxx_compiler}',
        },
        working_dir='mongo-cxx-driver',
        script='''\
            set -o errexit
            set -o pipefail
            cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="$(pwd)/../mongoc" -DCMAKE_CXX_STANDARD=20
            cmake --build build --target microbenchmarks --parallel 64
        '''
    )

    @classmethod
    def call(cls, **kwargs):
        return cls.default_call(**kwargs)


def functions():
    return CompileBenchmarks.defn()
