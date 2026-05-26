from shrub.v3.evg_command import EvgCommandType

from config_generator.etc.function import Function
from config_generator.etc.utils import bash_exec


class StopMongod(Function):
    name = 'stop_mongod'
    commands = [
        bash_exec(
            command_type=EvgCommandType.SYSTEM,
            script="""\
            set -o errexit
            if test -d drivers-evergreen-tools; then
                pushd drivers-evergreen-tools
                .evergreen/run-mongodb.sh stop
            fi
            """,
        )
    ]


def functions():
    return StopMongod.defn()
