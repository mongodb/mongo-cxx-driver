from shrub.v3.evg_command import EvgCommandType

from config_generator.etc.distros import compiler_to_vars
from config_generator.etc.function import Function
from config_generator.etc.utils import bash_exec

from typing import Mapping


class Test(Function):
    name = 'test'
    commands = bash_exec(
        command_type=EvgCommandType.TEST,
        include_expansions_in_env=[
            'build_type',
            'CRYPT_SHARED_LIB_PATH',  # Set by run-orchestration.sh in "start_mongod".
            'cse_aws_access_key_id',
            'cse_aws_secret_access_key',
            'cse_azure_client_id',
            'cse_azure_client_secret',
            'cse_azure_tenant_id',
            'cse_gcp_email',
            'cse_gcp_privatekey',
            'disable_slow_tests',
            'distro_id',
            'example_projects_cc',
            'example_projects_cxx',
            'example_projects_cxx_standard',
            'example_projects_cxxflags',
            'example_projects_ldflags',
            'generator',
            'MONGOCXX_TEST_TOPOLOGY',
            'MONGODB_API_VERSION',
            'platform',
            'TEST_WITH_ASAN',
            'TEST_WITH_UBSAN',
            'TEST_WITH_VALGRIND',
            'use_mongocryptd',
            'USE_STATIC_LIBS',
        ],
        working_dir='mongo-cxx-driver',
        script='.evergreen/scripts/test.sh',
    )

    @classmethod
    def call(cls, build_type: str | None = None, compiler: str | None = None, vars: Mapping[str, str] = None):
        vars = dict(vars or {})

        vars |= {'build_type': build_type} if build_type else {}
        vars |= compiler_to_vars(compiler)

        return cls.default_call(vars=vars if vars else None)


def functions():
    return Test.defn()
