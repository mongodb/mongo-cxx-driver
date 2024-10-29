from config_generator.components.funcs.compile import Compile
from config_generator.components.funcs.fetch_c_driver_source import FetchCDriverSource
from config_generator.components.funcs.fetch_det import FetchDET
from config_generator.components.funcs.install_c_driver import InstallCDriver
from config_generator.components.funcs.run_kms_servers import RunKMSServers
from config_generator.components.funcs.setup import Setup
from config_generator.components.funcs.start_mongod import StartMongod
from config_generator.components.funcs.test import Test

from config_generator.etc.distros import find_large_distro, make_distro_str

from shrub.v3.evg_build_variant import BuildVariant, DisplayTask
from shrub.v3.evg_command import KeyValueParam, expansions_update
from shrub.v3.evg_task import EvgTask, EvgTaskRef

from itertools import product


TAG = 'valgrind'


# pylint: disable=line-too-long
# fmt: off
MATRIX = [
    ('ubuntu1804', None, ['shared', 'static'], [False, True], ['5.0'   ], ['single']),
    ('ubuntu2004', None, ['shared', 'static'], [False, True], ['latest'], ['single']),
]
# fmt: on
# pylint: enable=line-too-long


def tasks():
    res = []

    for distro_name, compiler, link_types, with_extra_aligns, mongodb_versions, topologies in MATRIX:
        for link_type, with_extra_align, mongodb_version, topology in product(
            link_types, with_extra_aligns, mongodb_versions, topologies
        ):
            distro = find_large_distro(distro_name)

            name = f'{TAG}-{make_distro_str(distro_name, compiler, None)}'
            tags = [TAG, distro_name]

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
            compile_vars = {'BSON_EXTRA_ALIGNMENT': 1} if with_extra_align else {}
            test_vars = {
                'MONGOCXX_TEST_TOPOLOGY': topology,
                'TEST_WITH_VALGRIND': 'ON',
                'disable_slow_tests': 1,
                'use_mongocryptd': True,  # False positives arise from the crypt_shared library.
            }

            if link_type == 'static':
                updates.append(KeyValueParam(key='USE_STATIC_LIBS', value='1'))

            if not with_extra_align:
                compile_vars |= {'RUN_DISTCHECK': 1}

            commands = [expansions_update(updates=updates)] if updates else []

            commands += [
                Setup.call(),
                StartMongod.call(mongodb_version=mongodb_version, topology=topology),
                FetchCDriverSource.call(),
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
            display_tasks=[
                DisplayTask(
                    name=f'{TAG}-matrix',
                    execution_tasks=[f'.{TAG}'],
                )
            ],
        ),
    ]
