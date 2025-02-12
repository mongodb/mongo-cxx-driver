from config_generator.components.funcs.install_uv import InstallUV
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
    distro_name = 'rhel80'
    distro = find_small_distro(distro_name)

    return [
        EvgTask(
            name=TAG,
            tags=[TAG],
            run_on=distro.name,
            commands=[
                Setup.call(),
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
