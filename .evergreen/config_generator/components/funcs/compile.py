from shrub.v3.evg_command import EvgCommandType

from config_generator.etc.distros import compiler_to_vars
from config_generator.etc.function import Function
from config_generator.etc.utils import bash_exec

from typing import Mapping


class Compile(Function):
    name = 'compile'
    commands = bash_exec(
        command_type=EvgCommandType.TEST,
        env={
            'CC': '${cc_compiler}',
            'CXX': '${cxx_compiler}',
        },
        include_expansions_in_env=[
            'branch_name',
            'BSONCXX_POLYFILL',
            'build_type',
            'COMPILE_MACRO_GUARD_TESTS',
            'distro_id',
            'ENABLE_CODE_COVERAGE',
            'ENABLE_TESTS',
            'generator',
            'platform',
            'REQUIRED_CXX_STANDARD',
            'RUN_DISTCHECK',
            'USE_SANITIZER_ASAN',
            'USE_SANITIZER_UBSAN',
            'USE_STATIC_LIBS',
            'UV_INSTALL_DIR',
        ],
        working_dir='mongo-cxx-driver',
        script='.evergreen/scripts/compile.sh',
    )

    @classmethod
    def call(
        cls,
        build_type: str | None = None,
        compiler: str | None = None,
        polyfill: str | None = None,
        vars: Mapping[str, str] = {}
    ):
        vars = vars if vars else {}

        vars |= {'build_type': build_type} if build_type else {}
        vars |= compiler_to_vars(compiler)
        vars |= {'BSONCXX_POLYFILL': polyfill} if polyfill else {}

        return cls.default_call(vars=vars if vars else None)


def functions():
    return Compile.defn()
