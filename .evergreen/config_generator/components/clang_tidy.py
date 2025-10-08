from config_generator.components.funcs.install_c_driver import InstallCDriver
from config_generator.components.funcs.install_uv import InstallUV
from config_generator.components.funcs.setup import Setup

from config_generator.etc.distros import compiler_to_vars, find_small_distro
from config_generator.etc.function import Function
from config_generator.etc.utils import bash_exec

from shrub.v3.evg_build_variant import BuildVariant
from shrub.v3.evg_command import EvgCommandType
from shrub.v3.evg_task import EvgTask, EvgTaskRef

from typing import Mapping


TAG = 'clang-tidy'


class ClangTidy(Function):
    name = TAG
    commands = bash_exec(
        command_type=EvgCommandType.TEST,
        working_dir='mongo-cxx-driver',
        include_expansions_in_env=[
            'cc_compiler',
            'cxx_compiler',
            'distro_id',
            'UV_INSTALL_DIR',
        ],
        script='etc/run-clang-tidy.sh',
    )

    @classmethod
    def call(cls, compiler: str, vars: Mapping[str, str] = None):
        vars = dict(vars or {})

        vars |= compiler_to_vars(compiler)

        return cls.default_call(vars=vars)


def functions():
    return ClangTidy.defn()


def tasks():
    distro_name = 'rhel80'
    distro = find_small_distro(distro_name)

    return [
        EvgTask(
            name=TAG,
            tags=[TAG, distro_name],
            run_on=distro.name,
            commands=[
                Setup.call(),
                InstallUV.call(),
                InstallCDriver.call(compiler='clang'),
                ClangTidy.call(compiler='clang'),
            ],
        ),
    ]


def variants():
    return [
        BuildVariant(
            name=TAG,
            display_name='Clang Tidy',
            tasks=[EvgTaskRef(name=f'.{TAG}')],
        ),
    ]
