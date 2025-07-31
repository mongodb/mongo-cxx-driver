from config_generator.components.funcs.compile import Compile
from config_generator.components.funcs.install_uv import InstallUV
from config_generator.components.funcs.install_c_driver import InstallCDriver
from config_generator.components.funcs.setup import Setup

from config_generator.etc.distros import compiler_to_vars, find_large_distro, make_distro_str

from shrub.v3.evg_build_variant import BuildVariant
from shrub.v3.evg_command import KeyValueParam, expansions_update
from shrub.v3.evg_task import EvgTask, EvgTaskRef


TAG = 'compile-only'


# pylint: disable=line-too-long
# fmt: off
MATRIX = [
    # C++ standard and compiler coverage

    ('rhel80',     'clang',    [11, 17, 20,   ]), # Clang  7 (max: C++20)
    ('ubuntu2004', 'clang-10', [11, 17, 20,   ]), # Clang 10 (max: C++20)
    ('rhel84',     'clang',    [11, 17, 20,   ]), # Clang 11 (max: C++20)
    ('ubuntu2204', 'clang-12', [11, 17, 20, 23]), # Clang 12 (max: C++23)
    ('rhel90',     'clang',    [11, 17, 20, 23]), # Clang 13 (max: C++23)
    ('rhel91',     'clang',    [11, 17, 20, 23]), # Clang 14 (max: C++23)
    ('rhel92',     'clang',    [11, 17, 20, 23]), # Clang 15 (max: C++23)
    ('rhel93',     'clang',    [11, 17, 20, 23]), # Clang 16 (max: C++23)
    ('rhel94',     'clang',    [11, 17, 20, 23]), # Clang 17 (max: C++23)
    ('rhel95',     'clang',    [11, 17, 20, 23]), # Clang 19 (max: C++23)

    ('rhel76',     'gcc',    [11, 14,       ]), # GCC  4.8 (max: C++14)
    ('rhel80',     'gcc',    [11, 17, 20,   ]), # GCC  8.2 (max: C++20)
    ('debian10',   'gcc-8',  [11, 17, 20,   ]), # GCC  8.3 (max: C++20)
    ('rhel84',     'gcc',    [11, 17, 20,   ]), # GCC  8.4 (max: C++20)
    ('ubuntu2004', 'gcc-9',  [11, 17, 20,   ]), # GCC  9.4 (max: C++20)
    ('debian11',   'gcc-10', [11, 17, 20,   ]), # GCC 10.2 (max: C++20)
    ('rhel90',     'gcc',    [11, 17, 20, 23]), # GCC 11.2 (max: C++23)
    ('rhel92',     'gcc',    [11, 17, 20, 23]), # GCC 11.3 (max: C++23)
    ('rhel94',     'gcc',    [11, 17, 20, 23]), # GCC 11.4 (max: C++23)
    ('rhel95',     'gcc',    [11, 17, 20, 23]), # GCC 11.5 (max: C++23)

    ('windows-vsCurrent', 'vs2015x64', [11, 14,             'latest']), # Max: C++14
    ('windows-vsCurrent', 'vs2017x64', [11, 14, 17, 20,     'latest']), # Max: C++20
    ('windows-vsCurrent', 'vs2019x64', [11, 14, 17, 20, 23, 'latest']), # Max: C++23
    ('windows-vsCurrent', 'vs2022x64', [11, 14, 17, 20, 23, 'latest']), # Max: C++23

    # Other coverage.

    ('ubuntu2004-arm64', 'gcc',   [11, 17]), # Clang 10
    ('ubuntu2004-arm64', 'clang', [11, 17]), # Clang 10

    ('rhel8-power',   None, [11, 17]),
    ('rhel8-zseries', None, [11, 17]),

    ('macos-14-arm64', None, [11, 17]),
    ('macos-14',       None, [11, 17]),
]
# fmt: on
# pylint: enable=line-too-long


def tasks():
    build_type = 'Debug'

    for distro_name, compiler, cxx_standards in MATRIX:
        for cxx_standard in cxx_standards:
            distro = find_large_distro(distro_name)

            name = f'{TAG}-{make_distro_str(distro_name, compiler, None)}'
            tags = [TAG, distro_name]

            if cxx_standard is not None:
                name += f'-cxx{cxx_standard}'
                tags += [f'cxx{cxx_standard}']

            if compiler is not None:
                tags.append(compiler)

            name += f'-{build_type.lower()}'
            tags += [build_type.lower()]

            updates = []
            compile_vars = {'BUILD_SHARED_AND_STATIC_LIBS': 'ON'}

            updates += [KeyValueParam(key='build_type', value=build_type)]
            updates += [KeyValueParam(key=key, value=value) for key, value in compiler_to_vars(compiler).items()]

            if cxx_standard is not None:
                compile_vars |= {'REQUIRED_CXX_STANDARD': cxx_standard}

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
                    vars=compile_vars,
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
