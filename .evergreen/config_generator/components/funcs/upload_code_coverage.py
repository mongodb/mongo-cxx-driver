from shrub.v3.evg_command import EvgCommandType

from config_generator.etc.function import Function
from config_generator.etc.utils import bash_exec


class UploadCodeCoverage(Function):
    name = 'upload code coverage'
    commands = bash_exec(
        command_type=EvgCommandType.SYSTEM,
        include_expansions_in_env=['codecov_token'],
        working_dir='mongo-cxx-driver',
        script='.evergreen/scripts/upload-code-coverage.sh',
    )

    @classmethod
    def call(cls, **kwargs):
        return cls.default_call(**kwargs)


def functions():
    return UploadCodeCoverage.defn()
