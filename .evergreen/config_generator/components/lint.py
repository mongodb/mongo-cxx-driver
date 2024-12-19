from config_generator.components.funcs.install_uv import InstallUV
from config_generator.components.funcs.set_cache_dir import SetCacheDir
from config_generator.components.funcs.setup import Setup

from config_generator.etc.distros import find_small_distro
from config_generator.etc.function import Function
from config_generator.etc.utils import bash_exec

from shrub.v3.evg_build_variant import BuildVariant
from shrub.v3.evg_command import EvgCommandType
from shrub.v3.evg_task import EvgTask, EvgTaskRef


TAG = 'lint'


class Lint(Function):
    name = TAG
    commands = bash_exec(
        command_type=EvgCommandType.TEST,
        working_dir='mongo-cxx-driver',
        env={'DRYRUN': '1'},
        script='${UV_INSTALL_DIR}/uv run --frozen etc/clang-format-all.sh',
    )


def functions():
    return Lint.defn()


def tasks():
    distro_names = [
        'ubuntu2204',
        'ubuntu2004',
        'debian12',
        'debian11',
        'debian10',
    ]
    distros = [find_small_distro(name) for name in distro_names]

    return [
        EvgTask(
            name=TAG,
            tags=[TAG],
            run_on=[distro.name for distro in distros],
            commands=[
                Setup.call(),
                SetCacheDir.call(),
                InstallUV.call(),
                Lint.call(),
            ],
        ),
    ]


def variants():
    return [
        BuildVariant(
            name=TAG,
            display_name='Lint',
            tasks=[EvgTaskRef(name=f'.{TAG}')],
        ),
    ]
