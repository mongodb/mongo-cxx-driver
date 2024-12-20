from config_generator.etc.function import Function
from config_generator.etc.utils import bash_exec

from shrub.v3.evg_command import EvgCommandType, expansions_update


class InstallUV(Function):
    name = 'install-uv'
    commands = [
        bash_exec(
            command_type=EvgCommandType.SETUP,
            script='''\
                set -o errexit
                set -o pipefail

                if [[ ! -n "${MONGO_CXX_DRIVER_CACHE_DIR}" ]]; then
                    echo "MONGO_CXX_DRIVER_CACHE_DIR is not defined!" 1>&2
                    exit 1
                fi

                uv_install_dir="${MONGO_CXX_DRIVER_CACHE_DIR}/uv-0.5.9"
                mkdir -p "$uv_install_dir"

                if ! command -V "$uv_install_dir/uv" 2>/dev/null; then
                    env \\
                        UV_INSTALL_DIR="$uv_install_dir" \\
                        UV_NO_MODIFY_PATH=1 \\
                        mongo-cxx-driver/.evergreen/scripts/uv-installer.sh
                fi

                printf "UV_INSTALL_DIR: %s\\n" "$uv_install_dir" >|expansions.uv.yml
            ''',
        ),
        expansions_update(
            command_type=EvgCommandType.SETUP,
            file='expansions.uv.yml',
        ),
    ]


def functions():
    return InstallUV.defn()
