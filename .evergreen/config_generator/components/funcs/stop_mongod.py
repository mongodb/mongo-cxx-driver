from shrub.v3.evg_command import EvgCommandType

from config_generator.etc.function import Function
from config_generator.etc.utils import bash_exec


class StopMongod(Function):
    name = 'stop_mongod'
    commands = bash_exec(
        command_type=EvgCommandType.SYSTEM,
        script="""\
            set -o errexit
            set -o pipefail
            if cd drivers-evergreen-tools/.evergreen/orchestration 2>/dev/null; then
                . ../venv-utils.sh
                if venvactivate venv 2>/dev/null; then
                    mongo-orchestration stop
                fi
            fi
        """,
    )


def functions():
    return StopMongod.defn()
