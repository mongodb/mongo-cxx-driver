from shrub.v3.evg_command import EvgCommandType

from config_generator.etc.function import Function
from config_generator.etc.utils import bash_exec


class CSFLETeardown(Function):
    name = 'csfle-teardown'
    command_type = EvgCommandType.SETUP
    commands = [
        bash_exec(
            command_type=command_type,
            working_dir='drivers-evergreen-tools/.evergreen/csfle',
            script='./teardown.sh',
        ),
    ]


def functions():
    return CSFLETeardown.defn()
