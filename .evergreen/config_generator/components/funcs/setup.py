from config_generator.etc.function import Function
from config_generator.etc.utils import bash_exec

from shrub.v3.evg_command import EvgCommandType, git_get_project


class Setup(Function):
    name = 'setup'
    commands = [
        bash_exec(
            command_type=EvgCommandType.SETUP,
            script='''\
                set -o errexit
                set -o pipefail
                rm -rf "mongo-cxx-driver"
                rm -fr "mongo-c-driver"
                rm -fr mongod
                rm -fr drivers-evergreen-tools
            '''
        ),
        git_get_project(directory='mongo-cxx-driver'),
        bash_exec(
            command_type=EvgCommandType.SETUP,
            script='''\
                set -o errexit
                set -o pipefail
                cc --version || true
                c++ --version || true
                gcc --version || true
                g++ --version || true
                clang --version || true
                cmake --version || true
                openssl version || true
            '''
        ),
    ]


def functions():
    return Setup.defn()
