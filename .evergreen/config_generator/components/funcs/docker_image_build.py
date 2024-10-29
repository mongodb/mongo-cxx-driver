from shrub.v3.evg_command import EvgCommandType

from config_generator.etc.function import Function
from config_generator.etc.utils import bash_exec


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

    @classmethod
    def call(cls, **kwargs):
        return cls.default_call(**kwargs)


def functions():
    return DockerImageBuild.defn()
