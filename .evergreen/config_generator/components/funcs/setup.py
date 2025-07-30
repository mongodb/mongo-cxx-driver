from config_generator.etc.function import Function

from shrub.v3.evg_command import git_get_project


class Setup(Function):
    name = 'setup'
    commands = git_get_project(directory='mongo-cxx-driver')


def functions():
    return Setup.defn()
