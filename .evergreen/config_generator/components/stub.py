from shrub.v3.evg_build_variant import BuildVariant
from shrub.v3.evg_command import subprocess_exec
from shrub.v3.evg_task import EvgTask

TASKS = [
    EvgTask(
        name='stub',
        run_on='ubuntu2004-large',
        commands=[subprocess_exec(binary='bash', args=['-c', 'true'])],
    ),
]

def tasks():
    return TASKS

def variants():
    return [
        BuildVariant(
            name='stub',
            display_name='stub',
            tasks=[task.get_task_ref() for task in TASKS],
        ),
    ]
