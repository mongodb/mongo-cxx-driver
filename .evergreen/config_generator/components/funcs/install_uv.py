from config_generator.etc.function import Function
from config_generator.etc.utils import bash_exec

from shrub.v3.evg_command import EvgCommandType


class InstallUV(Function):
    name = 'install-uv'
    commands = bash_exec(
        command_type=EvgCommandType.SETUP,
        script='curl -LsSf https://astral.sh/uv/install.sh | sh'
    )


def functions():
    return InstallUV.defn()
