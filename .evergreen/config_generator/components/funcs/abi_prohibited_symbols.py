from config_generator.etc.function import Function
from config_generator.etc.utils import bash_exec

from shrub.v3.evg_command import EvgCommandType


class AbiProhibitedSymbols(Function):
    name = 'abi-prohibited-symbols'
    commands = bash_exec(
        command_type=EvgCommandType.TEST,
        script='mongo-cxx-driver/.evergreen/scripts/abi-prohibited-symbols-test.sh'
    )

    @classmethod
    def call(cls, **kwargs):
        return cls.default_call(**kwargs)


def functions():
    return AbiProhibitedSymbols.defn()
