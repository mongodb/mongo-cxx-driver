from config_generator.etc.function import Function
from config_generator.etc.utils import bash_exec

from shrub.v3.evg_command import EvgCommandType


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

    @classmethod
    def call(cls, **kwargs):
        return cls.default_call(**kwargs)


def functions():
    return RunMongohouse.defn()
