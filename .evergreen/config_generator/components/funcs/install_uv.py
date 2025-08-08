from config_generator.components.funcs.set_cache_dir import SetCacheDir

from config_generator.etc.function import Function
from config_generator.etc.utils import bash_exec

from shrub.v3.evg_command import EvgCommandType, expansions_update


class InstallUV(Function):
    name = 'install-uv'
    commands = SetCacheDir.commands + [
        bash_exec(
            command_type=EvgCommandType.SETUP,
            script='''\
                set -o errexit
                set -o pipefail

                version="0.8.6"

                if [[ ! -n "${MONGO_CXX_DRIVER_CACHE_DIR}" ]]; then
                    echo "MONGO_CXX_DRIVER_CACHE_DIR is not defined!" 1>&2
                    exit 1
                fi

                uv_install_dir="${MONGO_CXX_DRIVER_CACHE_DIR}/uv-$version"
                mkdir -p "$uv_install_dir"

                # Install if the binary is missing or the incorrect version.
                if ! (command -v "$uv_install_dir/uv" >/dev/null && "$uv_install_dir/uv" --version 2>/dev/null | grep "$version"); then
                    script="$(mktemp)"
                    cp -f mongo-cxx-driver/.evergreen/scripts/uv-installer.sh "$script"
                    chmod +x "$script"
                    # Always patch the install script so it validates checksums.
                    (
                      . mongo-cxx-driver/.evergreen/scripts/patch-uv-installer.sh
                      patch_uv_installer "$script" "$version"
                    )
                    env \\
                        UV_INSTALL_DIR="$uv_install_dir" \\
                        UV_UNMANAGED_INSTALL=1 \\
                        INSTALLER_PRINT_VERBOSE=1 \\
                        "$script"
                fi

                PATH="$uv_install_dir:$PATH" command -V uv
                PATH="$uv_install_dir:$PATH" uv --version

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
