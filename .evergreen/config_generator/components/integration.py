from config_generator.components.funcs.compile import Compile
from config_generator.components.funcs.fetch_det import FetchDET
from config_generator.components.funcs.install_c_driver import InstallCDriver
from config_generator.components.funcs.install_uv import InstallUV
from config_generator.components.funcs.run_kms_servers import RunKMSServers
from config_generator.components.funcs.setup import Setup
from config_generator.components.funcs.start_mongod import StartMongod
from config_generator.components.funcs.test import Test

from config_generator.etc.distros import compiler_to_vars, find_large_distro, make_distro_str

from shrub.v3.evg_build_variant import BuildVariant
from shrub.v3.evg_command import KeyValueParam, expansions_update
from shrub.v3.evg_task import EvgTask, EvgTaskRef

from itertools import product


TAG = 'integration'


# pylint: disable=line-too-long
# fmt: off
LINUX_MATRIX = [
    # Linux x86_64 (full).
    # RHEL 8 x86_64: 4.0+.
    ('rhel80', None, ['Debug'], ['shared', 'static'], [11, 17], [None], ['plain', 'csfle'], [False], [       '4.2', '4.4', '5.0', '6.0', '7.0', '8.0', 'latest'], ['single', 'replica', 'sharded']),
    ('rhel80', None, ['Debug'], ['shared', 'static'], [11, 17], [None], ['plain',        ], [False], ['4.0',                                                   ], ['single', 'replica', 'sharded']),  # CSFLE: 4.2+.

    # Linux ARM64 (full).
    # Linux ARM64: 4.4+.
    ('ubuntu2004-arm64', None, ['Debug'], ['shared', 'static'], [11, 17], [None], ['plain', 'csfle'], [False], ['4.4', '5.0', '6.0', '7.0', '8.0', 'latest'], ['single', 'replica', 'sharded']),

    # Linux Power.
    # RHEL 8 Power: 4.2+.
    ('rhel8-power', None, ['Debug'], ['shared'], [11], [None], ['csfle'], [False], ['latest'], ['replica']),

    # Linux zSeries.
    # RHEL 8 zSeries: 5.0+.
    ('rhel8-zseries', None, ['Debug'], ['shared'], [11], [None], ['csfle'], [False], ['latest'], ['replica']),
]

MACOS_MATRIX = [
    # MacOS ARM64 (shared only, no extra alignment, min-max-latest).
    # MacOS ARM64: 6.0+.
    ('macos-14-arm64', None, ['Debug'], ['shared'], [11, 17], [None], ['plain', 'csfle'], [False], ['6.0', '8.0', 'latest'], ['single', 'replica', 'sharded']),

    # MacOS x86_64 (shared only, C++11 only, no extra alignment, min-max-latest).
    # MacOS x86_64: 4.2+.
    ('macos-14', None, ['Debug'], ['shared'], [11], [None], ['plain', 'csfle'], [False], ['4.2', '8.0', 'latest'], ['single', 'replica', 'sharded']),

]

WINDOWS_MATRIX = [
    # Windows x86_64 (min-max-latest).
    # Windows x86_64: 4.2+.
    ('windows-vsCurrent', 'vs2022x64', ['Debug'], ['shared'], [11, 17], [None], ['plain', 'csfle'], [False], ['4.2', '8.0', 'latest'], ['single', 'replica', 'sharded']),
]

MONGOCRYPTD_MATRIX = [
    ('rhel80',            None,        ['Debug'], ['shared'], [11], [None], ['crypt'], [False], ['latest'], ['replica']),
    ('ubuntu2004-arm64',  None,        ['Debug'], ['shared'], [11], [None], ['crypt'], [False], ['latest'], ['replica']),
    ('rhel8-power',       None,        ['Debug'], ['shared'], [11], [None], ['crypt'], [False], ['latest'], ['replica']),
    ('rhel8-zseries',     None,        ['Debug'], ['shared'], [11], [None], ['crypt'], [False], ['latest'], ['replica']),
    ('macos-14-arm64',    None,        ['Debug'], ['shared'], [11], [None], ['crypt'], [False], ['latest'], ['replica']),
    ('macos-14',          None,        ['Debug'], ['shared'], [11], [None], ['crypt'], [False], ['latest'], ['replica']),
    ('windows-vsCurrent', 'vs2022x64', ['Debug'], ['shared'], [11], [None], ['crypt'], [False], ['latest'], ['replica']),
]

EXTRA_ALIGNMENT_MATRIX = [
    ('rhel80',            None,        ['Debug'], ['shared'], [11], [None], ['csfle'], [True], ['latest'], ['replica']),
    ('ubuntu2004-arm64',  None,        ['Debug'], ['shared'], [11], [None], ['csfle'], [True], ['latest'], ['replica']),
    ('rhel8-power',       None,        ['Debug'], ['shared'], [11], [None], ['csfle'], [True], ['latest'], ['replica']),
    ('rhel8-zseries',     None,        ['Debug'], ['shared'], [11], [None], ['csfle'], [True], ['latest'], ['replica']),
    ('macos-14-arm64',    None,        ['Debug'], ['shared'], [11], [None], ['csfle'], [True], ['latest'], ['replica']),
    ('macos-14',          None,        ['Debug'], ['shared'], [11], [None], ['csfle'], [True], ['latest'], ['replica']),
    ('windows-vsCurrent', 'vs2022x64', ['Debug'], ['shared'], [11], [None], ['csfle'], [True], ['latest'], ['replica']),
]

# fmt: on
# pylint: enable=line-too-long


ALL_MATRIX = LINUX_MATRIX + MACOS_MATRIX + WINDOWS_MATRIX + MONGOCRYPTD_MATRIX + EXTRA_ALIGNMENT_MATRIX


def tasks():
    for distro_name, compiler, build_types, link_types, cxx_standards, polyfills, with_csfles, with_extra_aligns, mongodb_versions, topologies in ALL_MATRIX:
        for build_type, link_type, cxx_standard, polyfill, with_csfle, with_extra_align, mongodb_version, topology in product(
            build_types, link_types, cxx_standards, polyfills, with_csfles, with_extra_aligns, mongodb_versions, topologies,
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

            if with_extra_align:
                name += '-extra_alignment'
                tags += ['extra_alignment']

            name += f'-{mongodb_version}-{topology}'
            tags += [mongodb_version, topology]

            if with_csfle == 'crypt':
                name += '-mongocryptd'
                tags += ['mongocryptd']

            updates = []
            icd_vars = {}
            compile_vars = {'ENABLE_TESTS': 'ON'}
            test_vars = {'MONGOCXX_TEST_TOPOLOGY': topology}

            if with_extra_align:
                icd_vars |= {'BSON_EXTRA_ALIGNMENT': 1}
                compile_vars |= {'BSON_EXTRA_ALIGNMENT': 1}
            else:
                compile_vars |= {'RUN_DISTCHECK': 1}

            if with_csfle != 'plain':
                test_vars |= {'TEST_WITH_CSFLE': 'ON'}

            updates += [KeyValueParam(key='build_type', value=build_type)]
            updates += [KeyValueParam(key=key, value=value) for key, value in compiler_to_vars(compiler).items()]

            if distro.os_type == 'windows':
                test_vars |= {'example_projects_cxx_standard': 17}

            if build_type == 'Debug' and distro.os in ['ubuntu1804', 'ubuntu2004', 'ubuntu2204']:
                updates += [KeyValueParam(key='ENABLE_CODE_COVERAGE', value='ON')]

            if link_type == 'static':
                compile_vars |= {'USE_STATIC_LIBS': 1}
                test_vars |= {'USE_STATIC_LIBS': 1}

            if cxx_standard is not None:
                compile_vars |= {'REQUIRED_CXX_STANDARD': cxx_standard}
                test_vars |= {
                    'example_projects_cxx_standard': cxx_standard,
                    'REQUIRED_CXX_STANDARD': cxx_standard,
                }

            commands = [expansions_update(updates=updates)] if updates else []
            commands += [
                Setup.call(),
                StartMongod.call(mongodb_version=mongodb_version, topology=topology),
            ] + [
                InstallCDriver.call(vars=icd_vars | ({'SKIP_INSTALL_LIBMONGOCRYPT': 1} if with_extra_align else {})),
            ] + [
                InstallUV.call(),
                Compile.call(polyfill=polyfill, vars=compile_vars),
                FetchDET.call(),
                RunKMSServers.call(),
            ] + [
                Test.call(vars=test_vars | ({'use_mongocryptd': True} if with_csfle == 'crypt' else {}))
            ]

            # PowerPC and zSeries are limited resources.
            patchable = False if any(pattern in distro_name for pattern in ['power', 'zseries']) else None

            yield EvgTask(
                name=name,
                tags=tags,
                run_on=distro.name,
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
        ('linux', '.linux !.mongocryptd !.extra_alignment', LINUX_MATRIX),
        ('macos', '.macos !.mongocryptd !.extra_alignment', MACOS_MATRIX),
        ('windows', '.windows !.mongocryptd !.extra_alignment', WINDOWS_MATRIX),
        ('mongocryptd', '.mongocryptd', MONGOCRYPTD_MATRIX),
        ('extra_alignment', '.extra_alignment', EXTRA_ALIGNMENT_MATRIX),
    ]

    for name, filter, matrix in matrices:
        distros = sorted(list({entry[0] for entry in matrix}))
        batched = [distro for distro in distros if distro in limited_distros]
        tasks = [
            EvgTaskRef(name=f'.{TAG} {filter} .{distro}', batchtime=one_day) for distro in batched
        ] + [
            EvgTaskRef(name=f'.{TAG} {filter}' + ''.join(f' !.{distro}' for distro in batched))
        ]

        yield BuildVariant(
            name=f'{TAG}-matrix-{name}',
            display_name=f'{TAG}-matrix-{name}',
            tasks=tasks,
        )
