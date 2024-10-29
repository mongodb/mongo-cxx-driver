from shrub.v3.evg_command import EvgCommandType

from config_generator.etc.function import Function
from config_generator.etc.utils import bash_exec


class CheckAugmentedSBOM(Function):
    name = 'check augmented sbom'
    commands = bash_exec(
        command_type=EvgCommandType.TEST,
        working_dir='mongo-cxx-driver',
        include_expansions_in_env=[
            'ARTIFACTORY_USER',
            'ARTIFACTORY_PASSWORD',
            'SILK_CLIENT_ID',
            'SILK_CLIENT_SECRET',
        ],
        script='.evergreen/scripts/check-augmented-sbom.sh',
    )

    @classmethod
    def call(cls, **kwargs):
        return cls.default_call(**kwargs)


def functions():
    return CheckAugmentedSBOM.defn()
