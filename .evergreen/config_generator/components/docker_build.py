from config_generator.components.funcs.setup import Setup

from config_generator.etc.distros import find_large_distro
from config_generator.etc.function import Function
from config_generator.etc.utils import bash_exec

from shrub.v3.evg_build_variant import BuildVariant
from shrub.v3.evg_command import EvgCommandType
from shrub.v3.evg_task import EvgTask, EvgTaskRef
from shrub.v3.evg_command import KeyValueParam, ec2_assume_role, expansions_update


TAG = 'docker-build'


# pylint: disable=line-too-long
# fmt: off
MATRIX = [
    ('ubuntu2204-arm64'), # `docker` is not available on RHEL distros by default.
    ('ubuntu2204'),       # `docker` is not available on RHEL distros by default.
]
# fmt: on
# pylint: enable=line-too-long


class DockerImageBuild(Function):
    name = 'docker-image-build'
    commands = [
        # Avoid inadvertently using a pre-existing and potentially conflicting Docker config.
        expansions_update(updates=[KeyValueParam(key='DOCKER_CONFIG', value='${workdir}/.docker')]),
        ec2_assume_role(role_arn='arn:aws:iam::901841024863:role/ecr-role-evergreen-ro'),
        bash_exec(
            command_type=EvgCommandType.SETUP,
            include_expansions_in_env=[
                "AWS_ACCESS_KEY_ID",
                "AWS_SECRET_ACCESS_KEY",
                "AWS_SESSION_TOKEN",
                "DOCKER_CONFIG",
            ],
            script='aws ecr get-login-password --region us-east-1 | docker login --username AWS --password-stdin 901841024863.dkr.ecr.us-east-1.amazonaws.com',
        ),
        bash_exec(
            command_type=EvgCommandType.TEST,
            working_dir='mongo-cxx-driver',
            env={
                # Use Amazon ECR as pull-through cache for DockerHub to avoid rate limits.
                "DEFAULT_SEARCH_REGISTRY": "901841024863.dkr.ecr.us-east-1.amazonaws.com/dockerhub",
            },
            include_expansions_in_env=['DOCKER_CONFIG'],
            script='''\
                set -o errexit
                set -o pipefail
                set -x
                echo "Building Alpine Docker image"
                make -C extras/docker/alpine3.19 nocachebuild test
                echo "Building Debian Docker image"
                make -C extras/docker/bookworm nocachebuild test
                echo "Building Red Hat UBI Docker image"
                make -C extras/docker/redhat-ubi-9.4 nocachebuild test
                echo "Building Ubuntu Docker image"
                make -C extras/docker/noble nocachebuild test
            ''',
        ),
    ]


def functions():
    return DockerImageBuild.defn()


def tasks():
    res = []

    for distro_name in MATRIX:
        distro = find_large_distro(distro_name)

        res.append(
            EvgTask(
                name=f'{TAG}-{distro_name}',
                tags=[TAG, distro_name],
                run_on=distro.name,
                commands=[
                    Setup.call(),
                    DockerImageBuild.call(),
                ],
            )
        )

    return res


def variants():
    return [
        BuildVariant(
            name=TAG,
            display_name='Docker Build',
            tasks=[EvgTaskRef(name=f'.{TAG}')],
        ),
    ]
