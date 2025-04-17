from config_generator.components.funcs.set_cache_dir import SetCacheDir

from config_generator.etc.function import Function
from config_generator.etc.utils import bash_exec

from shrub.v3.evg_command import EvgCommandType, expansions_update


class InstallValgrind(Function):
    name = 'install-valgrind'
    commands = SetCacheDir.commands + [
        bash_exec(
            command_type=EvgCommandType.SETUP,
            script='''\
                set -o errexit
                set -o pipefail

                if [[ ! -n "${MONGO_CXX_DRIVER_CACHE_DIR}" ]]; then
                  echo "MONGO_CXX_DRIVER_CACHE_DIR is not defined!" 1>&2
                  exit 1
                fi

                valgrind_install_dir="${MONGO_CXX_DRIVER_CACHE_DIR}/valgrind-3.24.0"
                mkdir -p "$valgrind_install_dir"

                if ! command -v "$valgrind_install_dir/bin/valgrind" 2>/dev/null; then
                  env \\
                    install_prefix="${MONGO_CXX_DRIVER_CACHE_DIR}/valgrind-3.24.0" \\
                    mongo-cxx-driver/.evergreen/scripts/valgrind-installer.sh
                fi

                PATH="$valgrind_install_dir/bin:$PATH" command -V valgrind
                PATH="$valgrind_install_dir/bin:$PATH" valgrind --version

                printf "VALGRIND_INSTALL_DIR: %s\\n" "$valgrind_install_dir/bin" >|expansions.valgrind.yml
            ''',
        ),
        expansions_update(
            command_type=EvgCommandType.SETUP,
            file='expansions.valgrind.yml',
        ),
    ]


def functions():
    return InstallValgrind.defn()
