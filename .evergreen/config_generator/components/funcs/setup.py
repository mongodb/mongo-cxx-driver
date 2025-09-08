from shrub.v3.evg_command import git_get_project

from config_generator.etc.function import Function


class Setup(Function):
    name = 'setup'
    commands = git_get_project(directory='mongo-cxx-driver')


def functions():
    return Setup.defn()
