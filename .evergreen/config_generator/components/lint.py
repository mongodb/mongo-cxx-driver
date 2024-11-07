from config_generator.components.funcs.setup import Setup

from config_generator.etc.distros import find_large_distro
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
        script='python etc/clang_format.py lint',
    )


def functions():
    return Lint.defn()


def tasks():
    distro_name = 'ubuntu1804'
    distro = find_large_distro(distro_name)

    return [
        EvgTask(
            name=TAG,
            tags=[TAG, distro_name],
            run_on=distro.name,
            commands=[
                Setup.call(),
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
