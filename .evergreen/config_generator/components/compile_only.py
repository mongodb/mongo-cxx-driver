from config_generator.components.funcs.compile import Compile
from config_generator.components.funcs.install_uv import InstallUV
from config_generator.components.funcs.install_c_driver import InstallCDriver
from config_generator.components.funcs.setup import Setup

from config_generator.etc.distros import compiler_to_vars, find_large_distro, make_distro_str

from shrub.v3.evg_build_variant import BuildVariant
from shrub.v3.evg_command import KeyValueParam, expansions_update
from shrub.v3.evg_task import EvgTask, EvgTaskRef

from itertools import product


TAG = 'compile-only'


# pylint: disable=line-too-long
# fmt: off
MATRIX = [
    ('rhel80', 'gcc',   ['Debug', 'Release'], ['shared', 'static'], [11, 17, 20, 23]),
    ('rhel80', 'clang', ['Debug', 'Release'], ['shared', 'static'], [11, 17, 20, 23]),

    ('ubuntu2004-arm64', 'gcc',   ['Debug', 'Release'], ['shared', 'static'], [11, 17, 20, 23]),
    ('ubuntu2004-arm64', 'clang', ['Debug', 'Release'], ['shared', 'static'], [11, 17, 20, 23]),

    ('rhel8-power',   None, ['Debug', 'Release'], ['shared', 'static'], [11, 17]),
    ('rhel8-zseries', None, ['Debug', 'Release'], ['shared', 'static'], [11, 17]),

    ('macos-14-arm64', None, ['Debug', 'Release'], ['shared', 'static'], [11, 17]),
    ('macos-14',       None, ['Debug', 'Release'], ['shared', 'static'], [11, 17]),

    ('windows-vsCurrent', 'vs2017x64', ['Debug', 'Release'], ['shared', 'static'],  [11, 17,       ]),
    ('windows-vsCurrent', 'vs2019x64', ['Debug', 'Release'], ['shared', 'static'],  [11, 17, 20,   ]),
    ('windows-vsCurrent', 'vs2022x64', ['Debug', 'Release'], ['shared', 'static'],  [11, 17, 20, 23]),
]
# fmt: on
# pylint: enable=line-too-long


def tasks():
    for distro_name, compiler, build_types, link_types, cxx_standards in MATRIX:
        for build_type, link_type, cxx_standard in product(build_types, link_types, cxx_standards):
            distro = find_large_distro(distro_name)

            name = f'{TAG}-{make_distro_str(distro_name, compiler, None)}'
            tags = [TAG, distro_name]

            if cxx_standard is not None:
                name += f'-cxx{cxx_standard}'
                tags += [f'cxx{cxx_standard}']

            if compiler is not None:
                tags.append(compiler)

            name += f'-{build_type.lower()}-{link_type}'
            tags += [build_type.lower(), link_type]

            updates = []
            compile_vars = {}

            updates += [KeyValueParam(key='build_type', value=build_type)]
            updates += [KeyValueParam(key=key, value=value) for key, value in compiler_to_vars(compiler).items()]

            if cxx_standard is not None:
                compile_vars |= {'REQUIRED_CXX_STANDARD': cxx_standard}

            if link_type == 'static':
                compile_vars |= {'USE_STATIC_LIBS': 1}

            # PowerPC and zSeries are limited resources.
            patchable = False if any(pattern in distro_name for pattern in ['power', 'zseries']) else None

            commands = [expansions_update(updates=updates)] if updates else []
            commands += [
                Setup.call(),
                InstallCDriver.call(),
                InstallUV.call(),
                Compile.call(
                    build_type=build_type,
                    compiler=compiler,
                )
            ]

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
    ]

    distros = sorted(list({entry[0] for entry in MATRIX}))
    batched = [distro for distro in distros if distro in limited_distros]
    tasks = [
        EvgTaskRef(name=f'.{TAG} .{distro}', batchtime=one_day) for distro in batched
    ] + [
        EvgTaskRef(name=f'.{TAG}' + ''.join(f' !.{distro}' for distro in batched))
    ]

    yield BuildVariant(
        name=f'{TAG}-matrix',
        display_name=f'{TAG}-matrix',
        tasks=tasks,
    )
