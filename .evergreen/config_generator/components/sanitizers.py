from config_generator.components.funcs.compile import Compile
from config_generator.components.funcs.fetch_det import FetchDET
from config_generator.components.funcs.install_c_driver import InstallCDriver
from config_generator.components.funcs.install_uv import InstallUV
from config_generator.components.funcs.run_kms_servers import RunKMSServers
from config_generator.components.funcs.setup import Setup
from config_generator.components.funcs.start_mongod import StartMongod
from config_generator.components.funcs.test import Test

from config_generator.etc.distros import find_large_distro, make_distro_str

from shrub.v3.evg_build_variant import BuildVariant
from shrub.v3.evg_command import KeyValueParam, expansions_update
from shrub.v3.evg_task import EvgTask, EvgTaskRef

from itertools import product


TAG = 'sanitizers'


# pylint: disable=line-too-long
# fmt: off
MATRIX = [
    ('rhel80', ['asan', 'ubsan'], ['shared', 'static'], ['4.0', '8.0', 'latest'], ['single', 'replica', 'sharded']),
]
# fmt: on
# pylint: enable=line-too-long


def tasks():
    res = []

    compiler = 'clang'
    cc_compiler = f'/opt/mongodbtoolchain/v4/bin/{compiler}'
    cxx_compiler = f'/opt/mongodbtoolchain/v4/bin/{compiler}++'

    for distro_name, sanitizers, link_types, mongodb_versions, topologies in MATRIX:
        for sanitizer, link_type, mongodb_version, topology in product(
            sanitizers, link_types, mongodb_versions, topologies
        ):
            distro = find_large_distro(distro_name)

            name = f'{TAG}-{sanitizer}-{make_distro_str(distro_name, compiler, None)}'
            tags = [TAG, sanitizer, distro_name]

            if compiler:
                tags.append(compiler)

            name += f'-{link_type}'
            tags += [link_type]

            name += f'-{mongodb_version}-{topology}'
            tags += [mongodb_version, topology]

            updates = [KeyValueParam(key='build_type', value='Debug')]
            updates += [KeyValueParam(key=key, value=value)
                        for key, value in [('cc_compiler', cc_compiler), ('cxx_compiler', cxx_compiler)]]

            icd_vars = {'SKIP_INSTALL_LIBMONGOCRYPT': 1}
            compile_vars = {'ENABLE_TESTS': 'ON', 'RUN_DISTCHECK': 1}
            test_vars = {
                'MONGOCXX_TEST_TOPOLOGY': topology,
                'example_projects_cc': cc_compiler,
                'example_projects_cxx': cxx_compiler,
            }

            if link_type == 'static':
                updates.append(KeyValueParam(key='USE_STATIC_LIBS', value='1'))

            commands = [expansions_update(updates=updates)] if updates else []

            match sanitizer:
                case 'asan':
                    compile_vars |= {
                        'USE_SANITIZER_ASAN': 'ON',
                    }

                    test_vars |= {
                        'TEST_WITH_ASAN': 'ON',
                        'example_projects_cxxflags': '-fsanitize=address -fno-omit-frame-pointer',
                        'example_projects_ldflags': '-fsanitize=address',
                    }

                case 'ubsan':
                    compile_vars |= {
                        'USE_SANITIZER_UBSAN': 'ON',
                    }

                    test_vars |= {
                        'TEST_WITH_UBSAN': 'ON',
                        'example_projects_cxxflags': '-fsanitize=undefined -fno-sanitize-recover=undefined -fno-omit-frame-pointer',
                        'example_projects_ldflags': '-fsanitize=undefined -fno-sanitize-recover=undefined' + (' -static-libsan' if link_type == 'static' else ''),
                    }

            commands += [
                Setup.call(),
                StartMongod.call(mongodb_version=mongodb_version, topology=topology),
                InstallUV.call(),
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
        ),
    ]
