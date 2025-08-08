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


TAG = 'valgrind'


# pylint: disable=line-too-long
# fmt: off
MATRIX = [
    # min-max-latest
    ('rhel80', None, ['shared'], ['4.2', '8.0', 'latest'], ['single', 'replica', 'sharded']),
]
# fmt: on
# pylint: enable=line-too-long


def tasks():
    res = []

    for distro_name, compiler, link_types, mongodb_versions, topologies in MATRIX:
        for link_type, mongodb_version, topology in product(
            link_types, mongodb_versions, topologies
        ):
            distro = find_large_distro(distro_name)

            name = f'{TAG}-{make_distro_str(distro_name, compiler, None)}'
            tags = [TAG, distro_name]

            if compiler:
                tags.append(compiler)

            name += f'-{link_type}'
            tags += [link_type]

            name += f'-{mongodb_version}-{topology}'
            tags += [mongodb_version, topology]

            updates = [KeyValueParam(key='build_type', value='Debug')]
            icd_vars = {'SKIP_INSTALL_LIBMONGOCRYPT': 1}
            compile_vars = {'ENABLE_TESTS': 'ON', 'RUN_DISTCHECK': 1}
            test_vars = {
                'MONGOCXX_TEST_TOPOLOGY': topology,
                'TEST_WITH_VALGRIND': 'ON',
                'disable_slow_tests': 1,
                'use_mongocryptd': True,  # False positives arise from the crypt_shared library.
            }

            if link_type == 'static':
                updates.append(KeyValueParam(key='USE_STATIC_LIBS', value='1'))

            commands = [expansions_update(updates=updates)] if updates else []

            commands += [
                Setup.call(),
                StartMongod.call(mongodb_version=mongodb_version, topology=topology),
                InstallUV.call(),
                InstallCDriver.call(vars=icd_vars),
                Compile.call(compiler=compiler, vars=compile_vars),
                FetchDET.call(),
                RunKMSServers.call(),
                Test.call(compiler=compiler, vars=test_vars),
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
