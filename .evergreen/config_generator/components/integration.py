from config_generator.components.funcs.compile import Compile
from config_generator.components.funcs.fetch_c_driver_source import FetchCDriverSource
from config_generator.components.funcs.fetch_det import FetchDET
from config_generator.components.funcs.install_c_driver import InstallCDriver
from config_generator.components.funcs.run_kms_servers import RunKMSServers
from config_generator.components.funcs.setup import Setup
from config_generator.components.funcs.start_mongod import StartMongod
from config_generator.components.funcs.test import Test

from config_generator.etc.distros import compiler_to_vars, find_large_distro, make_distro_str
from config_generator.etc.utils import TaskRef

from shrub.v3.evg_build_variant import BuildVariant, DisplayTask
from shrub.v3.evg_command import KeyValueParam, expansions_update
from shrub.v3.evg_task import EvgTask, EvgTaskRef

from itertools import product


TAG = 'integration'


# pylint: disable=line-too-long
# fmt: off
MATRIX = [
    ('debian10', None, ['Release'], ['shared', 'static'], [None], [None], ['plain'], [False, True], ['5.0'], ['single',                     ]),
    ('debian10', None, ['Release'], ['shared',         ], [None], [None], ['plain'], [False,     ], ['5.0'], [          'replica', 'sharded']),

    ('debian11', None, ['Release'], ['shared', 'static'], [None,   ], [None], ['plain'], [False, True], ['5.0'], ['single',                     ]),
    ('debian11', None, ['Release'], ['shared',         ], [      20], [None], ['plain'], [False, True], ['5.0'], ['single',                     ]),
    ('debian11', None, ['Release'], ['shared',         ], [None,   ], [None], ['plain'], [False,     ], ['5.0'], [          'replica', 'sharded']),

    ('debian12', None, ['Release'], ['shared', 'static'], [None,   ], [None], ['plain',        ], [False, True], ['latest'], ['single',                     ]),
    ('debian12', None, ['Release'], ['shared',         ], [      20], [None], ['plain',        ], [False, True], ['latest'], ['single',                     ]),
    ('debian12', None, ['Release'], ['shared',         ], [None,   ], [None], [         'csfle'], [False,     ], ['latest'], [          'replica', 'sharded']),

    ('macos-1100', None, ['Release'], ['shared', 'static'], [None], [None], ['plain'], [False, True], ['5.0', 'latest'], ['single']),

    ('rhel81-power8',  None, ['Release'], ['shared', 'static'], [None], [None], ['plain'], [False, True], ['5.0',        'latest'], ['single']),
    ('rhel83-zseries', None, ['Release'], ['shared', 'static'], [None], [None], ['plain'], [False, True], ['5.0', '6.0', 'latest'], ['single']),

    ('rhel90', None, ['Release'], ['shared', 'static'], [None,   ], [None], ['plain',        ], [False, True], ['latest'], ['single',                     ]),
    ('rhel90', None, ['Release'], ['shared',         ], [      20], [None], ['plain',        ], [False, True], ['latest'], ['single',                     ]),
    ('rhel90', None, ['Release'], ['shared',         ], [None,   ], [None], [         'csfle'], [False,     ], ['latest'], [          'replica', 'sharded']),

    ('rhel90-arm64', None, ['Release'], ['shared', 'static'], [None,   ], [None], ['plain',        ], [False, True], ['latest'], ['single',                     ]),
    ('rhel90-arm64', None, ['Release'], ['shared',         ], [      20], [None], ['plain',        ], [False, True], ['latest'], ['single',                     ]),
    ('rhel90-arm64', None, ['Release'], ['shared',         ], [None,   ], [None], [         'csfle'], [False,     ], ['latest'], [          'replica', 'sharded']),

    ('ubuntu1804', None, ['Debug',          ], ['shared'], [None], [None], ['plain',                 ], [False, True], ['4.0', '4.2', '4.4', '5.0', '6.0'], ['single',                     ]),
    ('ubuntu1804', None, ['Debug',          ], ['shared'], [None], [None], [         'csfle',        ], [False,     ], ['4.0', '4.2', '4.4', '5.0', '6.0'], [          'replica', 'sharded']),
    ('ubuntu1804', None, ['Debug',          ], ['shared'], [None], [None], [                  'crypt'], [False,     ], [       '4.2', '4.4', '5.0',      ], [          'replica', 'sharded']),
    ('ubuntu1804', None, [         'Release'], ['shared'], [None], [None], ['plain',                 ], [False,     ], [                     '5.0',      ], [          'replica', 'sharded']),

    ('ubuntu1804-arm64', None, ['Release'], ['shared', 'static'], [None], [None], ['plain'], [False, True], ['5.0'], ['single']),

    ('ubuntu2004', None, ['Debug',          ], ['shared',         ], [None], [None], ['plain',                 ], [False, True], [              '7.0', '8.0', 'latest'], ['single',                     ]),
    ('ubuntu2004', None, ['Debug',          ], ['shared',         ], [None], [None], [         'csfle',        ], [False,     ], [              '7.0', '8.0', 'latest'], [          'replica', 'sharded']),
    ('ubuntu2004', None, ['Debug',          ], ['shared',         ], [None], [None], [                  'crypt'], [False,     ], [                            'latest'], [          'replica', 'sharded']),
    ('ubuntu2004', None, [         'Release'], ['shared', 'static'], [None], [None], ['plain',                 ], [False, True], ['5.0',                      'latest'], ['single',                     ]),
    ('ubuntu2004', None, [         'Release'], ['shared',         ], [None], [None], [         'csfle',        ], [False,     ], ['5.0',                      'latest'], [          'replica', 'sharded']),
    ('ubuntu2004', None, [         'Release'], ['shared',         ], [None], [None], ['plain',                 ], [False,     ], ['5.0',                              ], [          'replica', 'sharded']),

    ('ubuntu2004-arm64', None, ['Release'], ['shared', 'static'], [None], [None], ['plain'], [False, True], ['latest'], ['single']),

    ('windows-vsCurrent', 'vs2019x64', ['Debug'], ['shared'], [None], [None], ['plain'], [False, True], ['4.0', '4.2', '4.4', '5.0', '6.0', '7.0', '8.0', 'latest'], ['single']),
]
# fmt: on
# pylint: enable=line-too-long


def tasks():
    res = []

    for distro_name, compiler, build_types, link_types, cxx_standards, polyfills, with_csfles, with_extra_aligns, mongodb_versions, topologies in MATRIX:
        for build_type, link_type, cxx_standard, polyfill, with_csfle, with_extra_align, mongodb_version, topology in product(
            build_types, link_types, cxx_standards, polyfills, with_csfles, with_extra_aligns, mongodb_versions, topologies,
        ):
            name = f'{TAG}-{make_distro_str(distro_name, compiler, None)}-{build_type.lower()}-{link_type}'
            tags = [TAG, distro_name, build_type.lower(), link_type]

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

            distro = find_large_distro(distro_name)

            updates = []
            icd_vars = {'BSON_EXTRA_ALIGNMENT': 1} if with_extra_align else {}
            compile_vars = {'BSON_EXTRA_ALIGNMENT': 1} if with_extra_align else {'RUN_DISTCHECK': 1}
            test_vars = {'MONGOCXX_TEST_TOPOLOGY': topology}

            updates += [KeyValueParam(key='build_type', value=build_type)]
            updates += [KeyValueParam(key=key, value=value) for key, value in compiler_to_vars(compiler).items()]

            if distro.os_type == 'windows':
                test_vars |= {'example_projects_cxx_standard': 17}

            if build_type == 'Debug' and distro.os in ['ubuntu1804', 'ubuntu2004']:
                updates += [KeyValueParam(key='ENABLE_CODE_COVERAGE', value='ON')]

            if 'rhel' in distro.os:
                test_vars |= {'lib_dir': 'lib64'}

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

            match with_csfle:
                case 'plain':
                    if with_extra_align:
                        commands += [
                            Setup.call(),
                            StartMongod.call(mongodb_version=mongodb_version, topology=topology),
                            InstallCDriver.call(vars=icd_vars | {'SKIP_INSTALL_LIBMONGOCRYPT': 1}),
                            Compile.call(polyfill=polyfill, vars=compile_vars),
                            FetchDET.call(),
                            RunKMSServers.call(),
                            Test.call(vars=test_vars),
                        ]
                    else:
                        commands += [
                            Setup.call(),
                            StartMongod.call(mongodb_version=mongodb_version, topology=topology),
                            FetchCDriverSource.call(),
                            Compile.call(polyfill=polyfill, vars=compile_vars),
                            FetchDET.call(),
                            RunKMSServers.call(),
                            Test.call(vars=test_vars),
                        ]
                case 'csfle':
                    commands += [
                        Setup.call(),
                        StartMongod.call(mongodb_version=mongodb_version, topology=topology),
                        InstallCDriver.call(vars=icd_vars),
                        Compile.call(polyfill=polyfill, vars=compile_vars),
                        FetchDET.call(),
                        RunKMSServers.call(),
                        Test.call(vars=test_vars),
                    ]
                case 'crypt':
                    commands += [
                        Setup.call(),
                        StartMongod.call(mongodb_version=mongodb_version, topology=topology),
                        InstallCDriver.call(vars=icd_vars),
                        Compile.call(polyfill=polyfill, vars=compile_vars),
                        FetchDET.call(),
                        RunKMSServers.call(),
                        Test.call(vars=test_vars | {'use_mongocryptd': True}),
                    ]

            # PowerPC and zSeries are limited resources.
            patchable = False if any(pattern in distro_name for pattern in ['power8', 'zseries']) else None

            res.append(
                EvgTask(
                    name=name,
                    tags=tags,
                    run_on=distro.name,
                    patchable=patchable,
                    commands=commands,
                )
            )

    return res


def variants():
    tasks = []

    one_day = 1440  # Seconds.

    # PowerPC and zSeries are limited resources.
    tasks = [
        TaskRef(name=f'.{TAG} .rhel81-power8', batchtime=one_day),
        TaskRef(name=f'.{TAG} .rhel83-zseries', batchtime=one_day),
        EvgTaskRef(name=f'.{TAG} !.rhel81-power8 !.rhel83-zseries'),
    ]

    return [
        BuildVariant(
            name=f'{TAG}-matrix',
            display_name=f'{TAG}-matrix',
            tasks=tasks,
        ),
    ]
