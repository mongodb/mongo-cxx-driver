from config_generator.components.funcs.compile import Compile
from config_generator.components.funcs.fetch_c_driver_source import FetchCDriverSource
from config_generator.components.funcs.install_uv import InstallUV
from config_generator.components.funcs.setup import Setup

from config_generator.etc.distros import find_large_distro, make_distro_str

from shrub.v3.evg_build_variant import BuildVariant
from shrub.v3.evg_task import EvgTask, EvgTaskRef


TAG = 'macro-guards'


# pylint: disable=line-too-long
# fmt: off
MATRIX = [
    ('rhel80', None   ),
    ('rhel80', 'gcc'  ),
    ('rhel80', 'clang'),
]
# fmt: on
# pylint: enable=line-too-long


def tasks():
    res = []

    for distro_name, compiler in MATRIX:
        distro = find_large_distro(distro_name)

        name = f'{TAG}-{make_distro_str(distro_name, compiler, None)}'
        tags = [TAG, distro_name]

        if compiler is not None:
            tags.append(compiler)

        res.append(
            EvgTask(
                name=name,
                tags=tags,
                run_on=distro.name,
                commands=[
                    Setup.call(),
                    FetchCDriverSource.call(),
                    InstallUV.call(),
                    Compile.call(
                        build_type='Debug',
                        compiler=compiler,
                        vars={'COMPILE_MACRO_GUARD_TESTS': 'ON', 'ENABLE_TESTS': 'ON'},
                    ),
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
