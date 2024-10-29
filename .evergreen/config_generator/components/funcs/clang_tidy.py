from config_generator.etc.function import Function
from config_generator.etc.utils import bash_exec

from shrub.v3.evg_command import EvgCommandType


class ClangTidy(Function):
    name = 'clang-tidy'
    commands = bash_exec(
        command_type=EvgCommandType.TEST,
        working_dir='mongo-cxx-driver',
        env={
            'CC': "${cc_compiler}",
            'CXX': "${cxx_compiler}",
        },
        include_expansions_in_env=['distro_id'],
        script='./etc/run-clang-tidy.sh',
    )

    @classmethod
    def call(cls, **kwargs):
        return cls.default_call(**kwargs)


def functions():
    return ClangTidy.defn()
