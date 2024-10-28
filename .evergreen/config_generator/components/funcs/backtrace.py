from shrub.v3.evg_command import EvgCommandType

from config_generator.etc.function import Function
from config_generator.etc.utils import bash_exec


class Backtrace(Function):
    name = 'backtrace'
    commands = bash_exec(
        command_type=EvgCommandType.SYSTEM,
        working_dir='mongo-cxx-driver',
        script='etc/debug-core-evergreen.sh',
    )

    @classmethod
    def call(cls, **kwargs):
        return cls.default_call(**kwargs)


def functions():
    return Backtrace.defn()
