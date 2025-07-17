from config_generator.components.funcs.setup import Setup

from config_generator.etc.distros import find_small_distro
from config_generator.etc.function import Function, merge_defns
from config_generator.etc.utils import bash_exec

from shrub.v3.evg_build_variant import BuildVariant
from shrub.v3.evg_command import (
    BuiltInCommand,
    EvgCommandType,
    KeyValueParam,
    ec2_assume_role,
    expansions_update,
    s3_put,
)
from shrub.v3.evg_task import EvgTask, EvgTaskRef

from pydantic import ConfigDict


TAG = 'sbom'


class CustomCommand(BuiltInCommand):
    command: str
    model_config = ConfigDict(arbitrary_types_allowed=True)


class CheckAugmentedSBOM(Function):
    name = 'check augmented sbom'
    commands = [
        # Authenticate with Kondukto.
        *[
            ec2_assume_role(
                command_type=EvgCommandType.SETUP,
                role_arn='${KONDUKTO_ROLE_ARN}',
            ),
            bash_exec(
                command_type=EvgCommandType.SETUP,
                include_expansions_in_env=['AWS_ACCESS_KEY_ID', 'AWS_SECRET_ACCESS_KEY', 'AWS_SESSION_TOKEN'],
                script='''\
                    set -o errexit
                    set -o pipefail
                    kondukto_token="$(aws secretsmanager get-secret-value --secret-id "kondukto-token" --region "us-east-1" --query 'SecretString' --output text)"
                    printf "KONDUKTO_TOKEN: %s\\n" "$kondukto_token" >|expansions.kondukto.yml
                ''',
            ),
            expansions_update(
                command_type=EvgCommandType.SETUP,
                file='expansions.kondukto.yml',
            ),
        ],
        # Authenticate with Amazon ECR.
        *[
            # Avoid inadvertently using a pre-existing and potentially conflicting Docker config.
            # Note: podman understands and uses DOCKER_CONFIG despite the name.
            expansions_update(updates=[KeyValueParam(key='DOCKER_CONFIG', value='${workdir}/.docker')]),
            ec2_assume_role(role_arn='arn:aws:iam::901841024863:role/ecr-role-evergreen-ro'),
            bash_exec(
                command_type=EvgCommandType.SETUP,
                include_expansions_in_env=[
                    'AWS_ACCESS_KEY_ID',
                    'AWS_SECRET_ACCESS_KEY',
                    'AWS_SESSION_TOKEN',
                    'DOCKER_CONFIG',
                ],
                script='aws ecr get-login-password --region us-east-1 | podman login --username AWS --password-stdin 901841024863.dkr.ecr.us-east-1.amazonaws.com',
            ),
        ],
        bash_exec(
            command_type=EvgCommandType.TEST,
            working_dir='mongo-cxx-driver',
            include_expansions_in_env=[
                'branch_name',
                'DOCKER_CONFIG',
                'KONDUKTO_TOKEN',
            ],
            script='.evergreen/scripts/sbom.sh',
        ),
    ]


class UploadAugmentedSBOM(Function):
    name = 'upload augmented sbom'
    commands = [
        # The current Augmented SBOM, ignoring version and timestamp fields.
        s3_put(
            command_type=EvgCommandType.SYSTEM,
            aws_key='${aws_key}',
            aws_secret='${aws_secret}',
            bucket='mciuploads',
            content_type='application/json',
            display_name='Augmented SBOM (Old)',
            local_file='mongo-cxx-driver/old.json',
            permissions='public-read',
            remote_file='mongo-cxx-driver/${build_variant}/${revision}/${version_id}/${build_id}/sbom/old.json',
        ),
        # The updated Augmented SBOM, ignoring version and timestamp fields.
        s3_put(
            command_type=EvgCommandType.SYSTEM,
            aws_key='${aws_key}',
            aws_secret='${aws_secret}',
            bucket='mciuploads',
            content_type='application/json',
            display_name='Augmented SBOM (New)',
            local_file='mongo-cxx-driver/new.json',
            permissions='public-read',
            remote_file='mongo-cxx-driver/${build_variant}/${revision}/${version_id}/${build_id}/sbom/new.json',
        ),
        # The difference between the current and updated Augmented SBOM.
        s3_put(
            command_type=EvgCommandType.SYSTEM,
            aws_key='${aws_key}',
            aws_secret='${aws_secret}',
            bucket='mciuploads',
            content_type='application/json',
            display_name='Augmented SBOM (Diff)',
            local_file='mongo-cxx-driver/diff.txt',
            permissions='public-read',
            remote_file='mongo-cxx-driver/${build_variant}/${revision}/${version_id}/${build_id}/sbom/diff.txt',
        ),
        # The updated Augmented SBOM without any filtering or modifications.
        s3_put(
            command_type=EvgCommandType.SYSTEM,
            aws_key='${aws_key}',
            aws_secret='${aws_secret}',
            bucket='mciuploads',
            content_type='application/json',
            display_name='Augmented SBOM (Updated)',
            local_file='mongo-cxx-driver/etc/augmented.sbom.json.new',
            permissions='public-read',
            remote_file='mongo-cxx-driver/${build_variant}/${revision}/${version_id}/${build_id}/sbom/augmented.sbom.json',
        ),
    ]


def functions():
    return merge_defns(
        CheckAugmentedSBOM.defn(),
        UploadAugmentedSBOM.defn(),
    )


def tasks():
    distro_name = 'rhel80'
    distro = find_small_distro(distro_name)

    yield EvgTask(
        name='sbom',
        tags=[TAG, distro_name],
        run_on=distro.name,
        commands=[
            Setup.call(),
            CheckAugmentedSBOM.call(),
            UploadAugmentedSBOM.call(),
        ],
    )


def variants():
    return [
        BuildVariant(
            name=TAG,
            display_name='SBOM',
            tasks=[EvgTaskRef(name=f'.{TAG}')],
        ),
    ]
