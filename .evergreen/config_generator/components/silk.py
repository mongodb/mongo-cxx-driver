from config_generator.components.funcs.setup import Setup

from config_generator.etc.distros import find_small_distro
from config_generator.etc.function import Function
from config_generator.etc.function import Function, merge_defns
from config_generator.etc.utils import bash_exec

from shrub.v3.evg_build_variant import BuildVariant
from shrub.v3.evg_command import EvgCommandType
from shrub.v3.evg_command import EvgCommandType, s3_put
from shrub.v3.evg_task import EvgTask, EvgTaskRef


TAG = 'silk'


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


class UploadAugmentedSBOM(Function):
    name = 'upload augmented sbom'
    commands = [
        s3_put(
            command_type=EvgCommandType.SYSTEM,
            aws_key='${aws_key}',
            aws_secret='${aws_secret}',
            bucket='mciuploads',
            content_type='application/json',
            display_name='Augmented SBOM',
            local_file='mongo-cxx-driver/etc/augmented.sbom.json.new',
            permissions='public-read',
            remote_file='mongo-cxx-driver/${build_variant}/${revision}/${version_id}/${build_id}/silk/augmented.sbom.json',
        ),
        s3_put(
            command_type=EvgCommandType.SYSTEM,
            aws_key='${aws_key}',
            aws_secret='${aws_secret}',
            bucket='mciuploads',
            content_type='application/json',
            display_name='Augmented SBOM (Diff)',
            local_file='mongo-cxx-driver/diff.txt',
            permissions='public-read',
            remote_file='mongo-cxx-driver/${build_variant}/${revision}/${version_id}/${build_id}/silk/augmented.sbom.json.diff',
        ),
    ]


def functions():
    return merge_defns(
        CheckAugmentedSBOM.defn(),
        UploadAugmentedSBOM.defn(),
    )


def tasks():
    distro_name = 'rhel8-latest'
    distro = find_small_distro(distro_name)

    return [
        EvgTask(
            name=TAG,
            tags=[TAG, distro_name],
            run_on=distro.name,
            commands=[
                Setup.call(),
                CheckAugmentedSBOM.call(),
                UploadAugmentedSBOM.call(),
            ],
        ),
    ]


def variants():
    return [
        BuildVariant(
            name=TAG,
            display_name='Silk',
            tasks=[EvgTaskRef(name=f'.{TAG}')],
        ),
    ]
