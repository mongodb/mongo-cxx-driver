from config_generator.components.funcs.setup import Setup

from config_generator.etc.distros import find_large_distro
from config_generator.etc.function import Function
from config_generator.etc.utils import bash_exec

from shrub.v3.evg_build_variant import BuildVariant
from shrub.v3.evg_command import EvgCommandType
from shrub.v3.evg_task import EvgTask, EvgTaskRef


TAG = 'docker-build'


# pylint: disable=line-too-long
# fmt: off
MATRIX = [
    ('ubuntu2204-arm64'),
    ('ubuntu2204'),
]
# fmt: on
# pylint: enable=line-too-long


class DockerImageBuild(Function):
    name = 'docker-image-build'
    commands = bash_exec(
        command_type=EvgCommandType.TEST,
        working_dir='mongo-cxx-driver',
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
        '''
    )


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
                ]
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
