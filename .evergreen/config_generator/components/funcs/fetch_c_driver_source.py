from config_generator.etc.function import Function
from config_generator.etc.utils import bash_exec

from shrub.v3.evg_command import EvgCommandType

# fetch_c_driver_source may be used to fetch the C driver source without installing the C driver.
# This can be used when only CI scripts are needed.


class FetchCDriverSource(Function):
    name = 'fetch_c_driver_source'
    commands = bash_exec(
        command_type=EvgCommandType.SETUP,
        script='git clone --depth 1 https://github.com/mongodb/mongo-c-driver mongoc'
    )

    @classmethod
    def call(cls, **kwargs):
        return cls.default_call(**kwargs)


def functions():
    return FetchCDriverSource.defn()
