from itertools import product

from shrub.v3.evg_build_variant import BuildVariant
from shrub.v3.evg_task import EvgTask, EvgTaskRef

from config_generator.components.funcs.compile import Compile
from config_generator.components.funcs.fetch_c_driver_source import FetchCDriverSource
from config_generator.components.funcs.fetch_det import FetchDET
from config_generator.components.funcs.run_kms_servers import RunKMSServers
from config_generator.components.funcs.setup import Setup
from config_generator.components.funcs.start_mongod import StartMongod
from config_generator.components.funcs.test import Test
from config_generator.etc.distros import find_large_distro, make_distro_str

TAG = 'versioned-api'


# pylint: disable=line-too-long
# fmt: off
MATRIX = [
    ('rhel80',            None,        ['Debug'], ['shared'], [None]),
    ('macos-14-arm64',    None,        ['Debug'], ['shared'], [None]),
    ('windows-vsCurrent', 'vs2022x64', ['Debug'], ['shared'], [None]),
]
# fmt: on
# pylint: enable=line-too-long


def tasks():
    res = []

    version_one_required = (
        'one-required',
        {'REQUIRE_API_VERSION': True},
        {'MONGODB_API_VERSION': 1},
    )

    version_two_accepted = (
        'two-accepted',
        {'ORCHESTRATION_FILE': 'versioned-api-testing.json'},
        {},
    )

    version_matrix = [version_one_required, version_two_accepted]

    for (distro_name, compiler, build_types, link_types, polyfills), (desc, mongod_vars, test_vars) in product(
        MATRIX, version_matrix
    ):
        distro = find_large_distro(distro_name)

        for build_type, link_type, polyfill in product(build_types, link_types, polyfills):
            name = f'{TAG}-{desc}-{make_distro_str(distro_name, compiler, None)}'
            tags = [TAG, distro_name]

            if compiler is not None:
                tags.append(compiler)

            name += f'-{build_type.lower()}-{link_type}'
            tags += [build_type.lower(), link_type]

            if polyfill is not None:
                name += f'-{polyfill}'
                tags.append(polyfill)

            mongod_vars |= {'AUTH': 'noauth'}  # Versioned API + auth is tested by the C Driver.
            compile_vars = {'ENABLE_TESTS': 'ON'}
            test_vars |= {'MONGOCXX_TEST_TOPOLOGY': 'single'}

            res.append(
                EvgTask(
                    name=name,
                    tags=tags,
                    run_on=distro.name,
                    commands=[
                        Setup.call(),
                        StartMongod.call(mongodb_version='latest', topology='single', vars=mongod_vars),
                        FetchCDriverSource.call(),
                        Compile.call(build_type=build_type, compiler=compiler, vars=compile_vars),
                        FetchDET.call(),
                        RunKMSServers.call(),
                        Test.call(build_type=build_type, compiler=compiler, vars=test_vars),
                    ],
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
