from shrub.v3.evg_command import EvgCommandType

from config_generator.etc.function import Function
from config_generator.etc.utils import bash_exec


class RunScanBuild(Function):
    name = 'run scan build'
    commands = [
        bash_exec(
            command_type=EvgCommandType.TEST,
            add_expansions_to_env=True,
            redirect_standard_error_to_output=True,
            working_dir='mongo-cxx-driver',
            script='.evergreen/scripts/compile-scan-build.sh',
        ),
    ]

    @classmethod
    def call(cls, **kwargs):
        return cls.default_call(**kwargs)


def functions():
    return RunScanBuild.defn()
