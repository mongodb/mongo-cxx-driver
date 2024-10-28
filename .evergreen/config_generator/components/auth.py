from config_generator.etc.utils import bash_exec
from config_generator.etc.function import Function
from shrub.v3.evg_command import EvgCommandType
from config_generator.components.funcs.compile import Compile
from config_generator.components.funcs.fetch_c_driver_source import FetchCDriverSource
from config_generator.components.funcs.setup import Setup
from config_generator.components.funcs.start_mongod import StartMongod

from config_generator.etc.distros import compiler_to_vars, find_large_distro, make_distro_str
from config_generator.etc.function import merge_defns

from shrub.v3.evg_build_variant import BuildVariant, DisplayTask
from shrub.v3.evg_command import KeyValueParam, expansions_update
from shrub.v3.evg_task import EvgTask, EvgTaskRef

from itertools import product


TAG = 'auth'


# pylint: disable=line-too-long
# fmt: off
MATRIX = [
    ('ubuntu2004', None, ['shared'], ['latest'], ['single']),

    ('windows-vsCurrent', 'vs2017x64', ['shared'], ['latest'], ['single']),
    ('windows-vsCurrent', 'vs2019x64', ['shared'], ['latest'], ['single']),
]
# fmt: on
# pylint: enable=line-too-long


class TestAuth(Function):
    name = 'test auth'
    commands = bash_exec(
        command_type=EvgCommandType.TEST,
        working_dir='mongo-cxx-driver',
        script='''\
            export MONGOC_INSTALL_PREFIX=$(pwd)/../mongoc
            export MONGOCXX_INSTALL_PREFIX=$(pwd)/build/install
            export BUILD_TYPE=${build_type}
            export BUILD_DIR=$(pwd)/build
            export URI="mongodb://bob:pwd123@localhost"

            .evergreen/scripts/connect.sh
        '''
    )

    @classmethod
    def call(cls, **kwargs):
        return cls.default_call(**kwargs)


class TestAtlasConnectivity(Function):
    name = 'test atlas connectivity'
    commands = bash_exec(
        command_type=EvgCommandType.TEST,
        silent=True,
        working_dir='mongo-cxx-driver',
        script='''\
            export MONGOC_INSTALL_PREFIX=$(pwd)/../mongoc
            export MONGOCXX_INSTALL_PREFIX=$(pwd)/build/install
            export BUILD_TYPE=${build_type}
            export BUILD_DIR=$(pwd)/build

            # The atlas_serverless_uri expansion is set in the Evergreen project settings.
            export URI="${atlas_serverless_uri}"

            .evergreen/scripts/connect.sh
        '''
    )

    @classmethod
    def call(cls, **kwargs):
        return cls.default_call(**kwargs)


def functions():
    return merge_defns(
        TestAuth.defn(),
        TestAtlasConnectivity.defn(),
    )


def tasks():
    res = []

    build_type = 'Debug'

    for distro_name, compiler, link_types, mongodb_versions, topologies in MATRIX:
        for link_type, mongodb_version, topology in product(link_types, mongodb_versions, topologies):
            name = f'{TAG}-{make_distro_str(distro_name, compiler, None)}'
            name += f'-{build_type.lower()}-{link_type}-{mongodb_version}-{topology}'

            tags = [TAG, distro_name]
            tags += [compiler] if compiler else []
            tags += [build_type.lower(), link_type, mongodb_version, topology]

            distro = find_large_distro(distro_name)

            updates = [KeyValueParam(key='build_type', value=build_type)]
            updates += [KeyValueParam(key=key, value=value) for key, value in compiler_to_vars(compiler).items()]

            compile_vars = {'ENABLE_CODE_COVERAGE': 'ON'} if distro.os != 'windows' else {}

            res.append(
                EvgTask(
                    name=name,
                    tags=tags,
                    run_on=distro.name,
                    commands=[
                        expansions_update(updates=updates),
                        Setup.call(),
                        StartMongod.call(mongodb_version=mongodb_version, topology=topology, vars={'AUTH': 'auth'}),
                        FetchCDriverSource.call(),
                        Compile.call(vars=compile_vars),
                        TestAuth.call(),
                        TestAtlasConnectivity.call(),
                    ],
                ),
            )

    return res


def variants():
    return [
        BuildVariant(
            name=f'{TAG}-matrix',
            display_name=f'{TAG}-matrix',
            tasks=[EvgTaskRef(name=f'.{TAG}')],
            display_tasks=[
                DisplayTask(
                    name=f'{TAG}-matrix',
                    execution_tasks=[f'.{TAG}'],
                )
            ],
        ),
    ]
