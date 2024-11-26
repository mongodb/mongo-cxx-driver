from config_generator.components.funcs.compile import Compile
from config_generator.components.funcs.fetch_det import FetchDET
from config_generator.components.funcs.install_c_driver import InstallCDriver
from config_generator.components.funcs.run_kms_servers import RunKMSServers
from config_generator.components.funcs.setup import Setup
from config_generator.components.funcs.start_mongod import StartMongod
from config_generator.components.funcs.test import Test

from config_generator.etc.distros import compiler_to_vars, find_large_distro, make_distro_str

from shrub.v3.evg_build_variant import BuildVariant, DisplayTask
from shrub.v3.evg_command import KeyValueParam, expansions_update
from shrub.v3.evg_task import EvgTask, EvgTaskRef

from itertools import product


TAG = 'sanitizers'


# pylint: disable=line-too-long
# fmt: off
MATRIX = [
    ('ubuntu1804', ['asan' ], ['shared', 'static'], [False, True], ['5.0'], ['single']),
    ('ubuntu1804', ['ubsan'], [          'static'], [False,     ], ['5.0'], ['single']),

    ('ubuntu2004', ['asan' ], ['shared', 'static'], [False, True], ['5.0'], ['single']),
    ('ubuntu2004', ['ubsan'], [          'static'], [False,     ], ['5.0'], ['single']),
]
# fmt: on
# pylint: enable=line-too-long


def tasks():
    res = []

    compiler = 'clang'

    for distro_name, sanitizers, link_types, with_extra_aligns, mongodb_versions, topologies in MATRIX:
        for sanitizer, link_type, with_extra_align, mongodb_version, topology in product(
            sanitizers, link_types, with_extra_aligns, mongodb_versions, topologies
        ):
            distro = find_large_distro(distro_name)

            name = f'{TAG}-{sanitizer}-{make_distro_str(distro_name, compiler, None)}'
            tags = [TAG, sanitizer, distro_name]

            if compiler:
                tags.append(compiler)

            name += f'-{link_type}'
            tags += [link_type]

            if with_extra_align:
                name += f'-extra_alignment'
                tags += ['extra_alignment']

            name += f'-{mongodb_version}-{topology}'
            tags += [mongodb_version, topology]

            updates = [KeyValueParam(key='build_type', value='Debug')]
            updates += [KeyValueParam(key=key, value=value) for key, value in compiler_to_vars(compiler).items()]

            icd_vars = {'SKIP_INSTALL_LIBMONGOCRYPT': 1}
            compile_vars = {'ENABLE_TESTS': 'ON'}
            test_vars = {
                'MONGOCXX_TEST_TOPOLOGY': topology,
                'example_projects_cc': 'clang',
                'example_projects_cxx': 'clang++',
            }

            if link_type == 'static':
                updates.append(KeyValueParam(key='USE_STATIC_LIBS', value='1'))

            if with_extra_align:
                icd_vars |= {'BSON_EXTRA_ALIGNMENT': 1}
                compile_vars |= {'BSON_EXTRA_ALIGNMENT': 1}
            else:
                compile_vars |= {'RUN_DISTCHECK': 1}

            commands = [expansions_update(updates=updates)] if updates else []

            match sanitizer:
                case 'asan':
                    compile_vars |= {
                        'USE_SANITIZER_ASAN': 'ON',
                    }

                    test_vars |= {
                        'TEST_WITH_ASAN': 'ON',
                        'example_projects_cxxflags': '-D_GLIBCXX_USE_CXX11_ABI=0 -fsanitize=address -fno-omit-frame-pointer',
                        'example_projects_ldflags': '-fsanitize=address',
                    }

                case 'ubsan':
                    compile_vars |= {
                        'USE_SANITIZER_UBSAN': 'ON',
                    }

                    test_vars |= {
                        'TEST_WITH_UBSAN': 'ON',
                        'example_projects_cxxflags': '-D_GLIBCXX_USE_CXX11_ABI=0 -fsanitize=undefined -fno-sanitize-recover=undefined -fno-omit-frame-pointer',
                        'example_projects_ldflags': '-fsanitize=undefined -fno-sanitize-recover=undefined',
                    }

            commands += [
                Setup.call(),
                StartMongod.call(mongodb_version=mongodb_version, topology=topology),
                InstallCDriver.call(vars=icd_vars),
                Compile.call(vars=compile_vars),
                FetchDET.call(),
                RunKMSServers.call(),
                Test.call(vars=test_vars),
            ]

            res.append(
                EvgTask(
                    name=name,
                    tags=tags,
                    run_on=distro.name,
                    commands=commands,
                )
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
