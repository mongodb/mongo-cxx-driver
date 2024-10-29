from config_generator.etc.function import Function
from config_generator.etc.utils import bash_exec

from shrub.v3.evg_command import EvgCommandType


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

    @classmethod
    def call(cls, **kwargs):
        return cls.default_call(**kwargs)


def functions():
    return BuildMongohouse.defn()
