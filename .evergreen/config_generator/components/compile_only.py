from config_generator.components.funcs.compile import Compile
from config_generator.components.funcs.fetch_c_driver_source import FetchCDriverSource
from config_generator.components.funcs.setup import Setup

from config_generator.etc.distros import find_large_distro, make_distro_str
from config_generator.etc.utils import TaskRef

from shrub.v3.evg_build_variant import BuildVariant, DisplayTask
from shrub.v3.evg_task import EvgTask, EvgTaskRef

from itertools import product


TAG = 'compile-only'


# pylint: disable=line-too-long
# fmt: off
MATRIX = [
    ('rhel79', None, ['Release'], ['shared'], ['impls']),

    ('rhel81-power8',  None, ['Release'], ['shared'],  [None]),
    ('rhel83-zseries', None, ['Release'], ['shared'],  [None]),

    ('ubuntu2004', None,    ['Debug'], ['shared'],  [None]),
    ('ubuntu2004', 'gcc',   ['Debug'], ['shared'],  [None]),
    ('ubuntu2004', 'clang', ['Debug'], ['shared'],  [None]),

    ('windows-64-vs2015', 'vs2015x64', ['Debug'], ['shared'],  [None]),
    ('windows-64-vs2015', 'vs2015x64', ['Release'], ['shared'],  [None]),
]
# fmt: on
# pylint: enable=line-too-long


def generate_tasks():
    res = []

    for distro_name, compiler, build_types, link_types, polyfills in MATRIX:
        for build_type, link_type, polyfill in product(build_types, link_types, polyfills):
            distro = find_large_distro(distro_name)

            name = f'{TAG}-{make_distro_str(distro_name, compiler, None)}'
            tags = [TAG, distro_name]

            if compiler is not None:
                tags.append(compiler)

            name += f'-{build_type.lower()}-{link_type}'
            tags += [build_type.lower(), link_type]

            if polyfill is not None:
                name += f'-{polyfill}'
                tags.append(polyfill)

            # PowerPC and zSeries are limited resources.
            patchable = False if any(pattern in distro_name for pattern in ['power8', 'zseries']) else None

            res.append(
                EvgTask(
                    name=name,
                    tags=tags,
                    run_on=distro.name,
                    patchable=patchable,
                    commands=[
                        Setup.call(),
                        FetchCDriverSource.call(),
                        Compile.call(
                            build_type=build_type,
                            compiler=compiler,
                            polyfill=polyfill,
                            vars={'ENABLE_TESTS': 'OFF'}
                        )
                    ],
                )
            )

    return res


TASKS = generate_tasks()


def tasks():
    res = TASKS.copy()

    # PowerPC and zSeries are limited resources.
    for task in res:
        if any(pattern in task.run_on for pattern in ["power8", "zseries"]):
            task.patchable = False

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
            display_tasks=[
                DisplayTask(
                    name=f'{TAG}-matrix',
                    execution_tasks=[f'.{TAG}'],
                )
            ],
        ),
    ]
