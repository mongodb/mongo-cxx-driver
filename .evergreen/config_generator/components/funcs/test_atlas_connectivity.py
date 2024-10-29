from shrub.v3.evg_command import EvgCommandType

from config_generator.etc.function import Function
from config_generator.etc.utils import bash_exec


class TestAtlasConnectivity(Function):
    name = 'test atlas connectivity'
    commands = bash_exec(
        command_type=EvgCommandType.TEST,
        silent=True,
        working_dir='mongo-cxx-driver',
        script='''\
            export MONGOC_INSTALL_PREFIX=$(pwd)/../mongoc
            export MONGOCXX_INSTALL_PREFIX=$(pwd)/build/install
            export LIB_DIR=${lib_dir}
            export BUILD_TYPE=${build_type}
            export BUILD_DIR=$(pwd)/build

            # The atlas_serverless_uri expansion is set in the Evergreen project settings.
            export URI="${atlas_serverless_uri}"

            ./.evergreen/scripts/connect.sh
        '''
    )

    @classmethod
    def call(cls, **kwargs):
        return cls.default_call(**kwargs)


def functions():
    return TestAtlasConnectivity.defn()