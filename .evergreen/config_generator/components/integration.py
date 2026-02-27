from itertools import product

from shrub.v3.evg_build_variant import BuildVariant
from shrub.v3.evg_command import KeyValueParam, expansions_update
from shrub.v3.evg_task import EvgTask, EvgTaskRef

from config_generator.components.funcs.compile import Compile
from config_generator.components.funcs.fetch_det import FetchDET
from config_generator.components.funcs.install_c_driver import InstallCDriver
from config_generator.components.funcs.run_kms_servers import RunKMSServers
from config_generator.components.funcs.setup import Setup
from config_generator.components.funcs.start_mongod import StartMongod
from config_generator.components.funcs.test import Test
from config_generator.etc.distros import compiler_to_vars, find_large_distro, make_distro_str

TAG = 'integration'


# pylint: disable=line-too-long
# fmt: off
LINUX_MATRIX = [
    # Linux x86_64 (full).
    # RHEL 8 x86_64: 4.0+.
    ('rhel80', None, ['Debug'], ['shared', 'static'], [11, 17], [None], ['plain', 'csfle'], ['4.2', '4.4', '5.0', '6.0', '7.0', '8.0', 'rapid', 'latest'], ['single', 'replica', 'sharded']),

    # Linux ARM64 (full).
    # RHEL 8 ARM64: 4.4+.
    ('rhel8-arm64-latest', None, ['Debug'], ['shared', 'static'], [11, 17], [None], ['plain', 'csfle'], ['4.4', '5.0', '6.0', '7.0', '8.0', 'rapid', 'latest'], ['single', 'replica', 'sharded']),

    # Linux Power.
    # RHEL 8 Power: 4.2+.
    ('rhel8-power', None, ['Debug'], ['shared'], [11], [None], ['csfle'], ['latest'], ['replica']),

    # Linux zSeries.
    # RHEL 8 zSeries: 5.0+.
    ('rhel8-zseries', None, ['Debug'], ['shared'], [11], [None], ['csfle'], ['latest'], ['replica']),
]

MACOS_MATRIX = [
    # MacOS ARM64 (shared only, no extra alignment, min-max-latest).
    # MacOS ARM64: 6.0+.
    ('macos-14-arm64', None, ['Debug'], ['shared'], [11, 17], [None], ['plain', 'csfle'], ['6.0', '8.0', 'latest'], ['single', 'replica', 'sharded']),

    # MacOS x86_64 (shared only, C++11 only, no extra alignment, min-max-latest).
    # MacOS x86_64: 4.2+.
    ('macos-14', None, ['Debug'], ['shared'], [11], [None], ['plain', 'csfle'], ['4.2', '8.0', 'latest'], ['single', 'replica', 'sharded']),

]

WINDOWS_MATRIX = [
    # Windows x86_64 (min-max-latest).
    # Windows x86_64: 4.2+.
    ('windows-vsCurrent',   'vs2022x64', ['Debug'], ['shared'], [11, 17], [None], ['plain', 'csfle'], ['4.2',                ], ['single', 'replica', 'sharded']),
    ('windows-2022-latest', 'vs2022x64', ['Debug'], ['shared'], [11, 17], [None], ['plain', 'csfle'], [       '8.0', 'latest'], ['single', 'replica', 'sharded']),

    ('windows-2022-latest', 'gcc',       ['Debug'], ['shared'], [11, 17], [None], ['plain'         ], ['4.2', '8.0', 'latest'], ['single', 'replica', 'sharded']),
]

MONGOCRYPTD_MATRIX = [
    ('rhel80',              None,        ['Debug'], ['shared'], [11], [None], ['crypt'], ['latest'], ['replica']),
    ('rhel8-arm64-latest',  None,        ['Debug'], ['shared'], [11], [None], ['crypt'], ['latest'], ['replica']),
    ('rhel8-power',         None,        ['Debug'], ['shared'], [11], [None], ['crypt'], ['latest'], ['replica']),
    ('rhel8-zseries',       None,        ['Debug'], ['shared'], [11], [None], ['crypt'], ['latest'], ['replica']),
    ('macos-14-arm64',      None,        ['Debug'], ['shared'], [11], [None], ['crypt'], ['latest'], ['replica']),
    ('macos-14',            None,        ['Debug'], ['shared'], [11], [None], ['crypt'], ['latest'], ['replica']),
    ('windows-2022-latest', 'vs2022x64', ['Debug'], ['shared'], [11], [None], ['crypt'], ['latest'], ['replica']),
]

# fmt: on
# pylint: enable=line-too-long


ALL_MATRIX = LINUX_MATRIX + MACOS_MATRIX + WINDOWS_MATRIX + MONGOCRYPTD_MATRIX


def tasks():
    for (
        distro_name,
        compiler,
        build_types,
        link_types,
        cxx_standards,
        polyfills,
        with_csfles,
        mongodb_versions,
        topologies,
    ) in ALL_MATRIX:
        for build_type, link_type, cxx_standard, polyfill, with_csfle, mongodb_version, topology in product(
            build_types,
            link_types,
            cxx_standards,
            polyfills,
            with_csfles,
            mongodb_versions,
            topologies,
        ):
            distro = find_large_distro(distro_name)

            name = f'{TAG}-{make_distro_str(distro_name, compiler, None)}-{build_type.lower()}-{link_type}'
            tags = [TAG, distro_name, distro.os_type, build_type.lower(), link_type]

            if cxx_standard is not None:
                name += f'-cxx{cxx_standard}'
                tags += [f'cxx{cxx_standard}']

            if polyfill is not None:
                name += f'-{polyfill}'
                tags += [polyfill]

            if with_csfle in ['csfle', 'crypt']:
                name += '-csfle'
                tags += ['csfle']

            name += f'-{mongodb_version}-{topology}'
            tags += [mongodb_version, topology]

            if with_csfle == 'crypt':
                name += '-mongocryptd'
                tags += ['mongocryptd']

            updates = []
            icd_vars = {}
            compile_vars = {'ENABLE_TESTS': 'ON', 'RUN_DISTCHECK': 1}
            test_vars = {'MONGOCXX_TEST_TOPOLOGY': topology}

            if with_csfle != 'plain':
                test_vars |= {'TEST_WITH_CSFLE': 'ON'}

            updates += [KeyValueParam(key='build_type', value=build_type)]
            updates += [KeyValueParam(key=key, value=value) for key, value in compiler_to_vars(compiler).items()]

            if build_type == 'Debug' and distro.os in ['ubuntu2204', 'ubuntu2404']:
                updates += [KeyValueParam(key='ENABLE_CODE_COVERAGE', value='ON')]

            if link_type == 'static':
                compile_vars |= {'USE_STATIC_LIBS': 1}
                test_vars |= {'USE_STATIC_LIBS': 1}

            if cxx_standard is not None:
                compile_vars |= {'REQUIRED_CXX_STANDARD': cxx_standard}
                test_vars |= {'REQUIRED_CXX_STANDARD': cxx_standard}

            # DEVPROD-1167: MacOS distros are sometimes much slower than usual.
            exec_timeout_secs = 7200 if distro.os_type == 'macos' else None

            commands = [expansions_update(updates=updates)] if updates else []
            commands += [
                Setup.call(),
                StartMongod.call(mongodb_version=mongodb_version, topology=topology),
                InstallCDriver.call(vars=icd_vars),
                Compile.call(polyfill=polyfill, vars=compile_vars),
                FetchDET.call(),
                RunKMSServers.call(),
                Test.call(vars=test_vars | ({'use_mongocryptd': True} if with_csfle == 'crypt' else {})),
            ]

            # PowerPC and zSeries are limited resources.
            patchable = False if any(pattern in distro_name for pattern in ['power', 'zseries']) else None

            yield EvgTask(
                name=name,
                tags=tags,
                run_on=distro.name,
                exec_timeout_secs=exec_timeout_secs,
                patchable=patchable,
                commands=commands,
            )


def variants():
    tasks = []

    one_day = 1440  # Seconds.

    # PowerPC and zSeries are limited resources.
    limited_distros = [
        'rhel8-power',
        'rhel8-zseries',
        'rhel7-zseries',
    ]

    matrices = [
        ('linux', '.linux !.mongocryptd', LINUX_MATRIX),
        ('macos', '.macos !.mongocryptd', MACOS_MATRIX),
        ('windows', '.windows !.mongocryptd', WINDOWS_MATRIX),
        ('mongocryptd', '.mongocryptd', MONGOCRYPTD_MATRIX),
    ]

    for name, filter, matrix in matrices:
        distros = sorted(list({entry[0] for entry in matrix}))
        batched = [distro for distro in distros if distro in limited_distros]
        tasks = [EvgTaskRef(name=f'.{TAG} {filter} .{distro}', batchtime=one_day) for distro in batched] + [
            EvgTaskRef(name=f'.{TAG} {filter}' + ''.join(f' !.{distro}' for distro in batched))
        ]

        yield BuildVariant(
            name=f'{TAG}-matrix-{name}',
            display_name=f'{TAG}-matrix-{name}',
            tasks=tasks,
        )
