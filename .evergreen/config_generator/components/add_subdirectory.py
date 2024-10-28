from config_generator.components.funcs.setup import Setup
from config_generator.components.funcs.start_mongod import StartMongod

from config_generator.etc.function import Function
from config_generator.etc.distros import find_large_distro, make_distro_str
from config_generator.etc.utils import bash_exec

from shrub.v3.evg_build_variant import BuildVariant, DisplayTask
from shrub.v3.evg_command import EvgCommandType
from shrub.v3.evg_task import EvgTask, EvgTaskRef

from itertools import product


TAG = 'add-subdirectory'


# pylint: disable=line-too-long
# fmt: off
MATRIX = [
    ('debian10',     ['5.0',         ], ['single']),
    ('debian11',     ['5.0',         ], ['single']),
    ('debian12',     [       'latest'], ['single']),
    ('rhel90',       [       'latest'], ['single']),
    ('rhel90-arm64', [       'latest'], ['single']),
    ('ubuntu1804',   ['5.0',         ], ['single']),
    ('ubuntu2004',   ['5.0', 'latest'], ['single']),
]
# fmt: on
# pylint: enable=line-too-long


class BuildExamplesWithAddSubdirectory(Function):
    name = 'build-examples-with-add-subdirectory'
    commands = bash_exec(
        command_type=EvgCommandType.TEST,
        working_dir='mongo-cxx-driver',
        script='.evergreen/scripts/build-examples-with-add-subdirectory.sh',
    )

    @classmethod
    def call(cls, **kwargs):
        return cls.default_call(**kwargs)


def functions():
    return BuildExamplesWithAddSubdirectory.defn()


def tasks():
    res = []

    build_type = 'Release'

    for distro_name, mongodb_versions, topologies in MATRIX:
        distro = find_large_distro(distro_name)

        for mongodb_version, topology in product(mongodb_versions, topologies):
            name = f'{TAG}-{make_distro_str(distro_name, None, None)}-{build_type.lower()}-{mongodb_version}-{topology}'
            tags = [TAG, distro_name, build_type.lower(), mongodb_version, topology]

            res.append(
                EvgTask(
                    name=name,
                    tags=tags,
                    run_on=distro.name,
                    commands=[
                        Setup.call(),
                        StartMongod.call(mongodb_version=mongodb_version, topology=topology),
                        BuildExamplesWithAddSubdirectory.call(),
                    ],
                )
            )

    return res


def variants():
    return [
        BuildVariant(
            name=f'{TAG}-examples',
            display_name=f'{TAG}-examples',
            tasks=[EvgTaskRef(name=f'.{TAG}')],
            display_tasks=[
                DisplayTask(
                    name=f'{TAG}-examples',
                    execution_tasks=[f'.{TAG}'],
                )
            ],
        ),
    ]
