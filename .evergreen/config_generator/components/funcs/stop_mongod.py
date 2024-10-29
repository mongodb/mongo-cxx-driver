from config_generator.etc.function import Function
from config_generator.etc.utils import bash_exec

from shrub.v3.evg_command import EvgCommandType


class StopMongod(Function):
    name = 'stop_mongod'
    commands = bash_exec(
        command_type=EvgCommandType.SYSTEM,
        script='''\
            set -o errexit
            set -o pipefail
            if cd drivers-evergreen-tools/.evergreen/orchestration 2>/dev/null; then
                . ../venv-utils.sh
                if venvactivate venv; then
                mongo-orchestration stop
                fi
            fi
        '''
    )

    @classmethod
    def call(cls, **kwargs):
        return cls.default_call(**kwargs)


def functions():
    return StopMongod.defn()
