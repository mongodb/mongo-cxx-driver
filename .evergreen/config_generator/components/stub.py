from shrub.v3.evg_build_variant import BuildVariant
from shrub.v3.evg_command import subprocess_exec
from shrub.v3.evg_task import EvgTask


def functions():
    return {
        'stub': [
            subprocess_exec(
                binary='bash',
                args=['-c', 'echo "stub!"'],
            )
        ]
    }


def tasks():
    return [
        EvgTask(
            name='stub',
            commands=[{'func': 'stub'}],
            run_on='ubuntu2404-large',
        )
    ]


def variants():
    return [
        BuildVariant(
            name='stub',
            display_name='stub',
            tasks=[{'name': 'stub'}],
        )
    ]
