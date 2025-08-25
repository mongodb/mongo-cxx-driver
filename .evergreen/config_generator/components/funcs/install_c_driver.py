from typing import Mapping

from shrub.v3.evg_command import EvgCommandType, KeyValueParam, expansions_update

from config_generator.etc.distros import compiler_to_vars
from config_generator.etc.function import Function
from config_generator.etc.utils import bash_exec

# If updating mongoc_version_minimum to a new release (not pinning to an unreleased commit), also update:
# - BSON_REQUIRED_VERSION and MONGOC_REQUIRED_VERSION in CMakeLists.txt
# - the version of pkg:github/mongodb/mongo-c-driver in etc/purls.txt
# - the default value of --c-driver-build-ref in etc/make_release.py
# If pinning to an unreleased commit, create a "Blocked" JIRA ticket with
# a "depends on" link to the appropriate C Driver version release ticket.
MONGOC_VERSION_MINIMUM = '912209d5dc985758bc3d70b105dc5166e3ded7c3'  # TODO: bump to 2.1.0 once released.


class InstallCDriver(Function):
    name = 'install_c_driver'
    commands = [
        expansions_update(
            command_type=EvgCommandType.SETUP,
            updates=[KeyValueParam(key='mongoc_version_minimum', value=MONGOC_VERSION_MINIMUM)],
        ),
        bash_exec(
            command_type=EvgCommandType.SETUP,
            add_expansions_to_env=True,
            env={
                'CC': '${cc_compiler}',
                'CXX': '${cxx_compiler}',
            },
            script='mongo-cxx-driver/.evergreen/scripts/install-c-driver.sh',
        ),
    ]

    @classmethod
    def call(cls, compiler: str | None = None, vars: Mapping[str, str] = None):
        vars = dict(vars or {})

        vars |= compiler_to_vars(compiler)

        return cls.default_call(vars=vars if vars else None)


def functions():
    return InstallCDriver.defn()
