from typing import Mapping

from shrub.v3.evg_command import EvgCommandType, ec2_assume_role, expansions_update

from config_generator.etc.function import Function
from config_generator.etc.utils import bash_exec


class StartMongod(Function):
    name = 'start_mongod'
    commands = [
        # Downloads of the "latest" server binary require an AWS identity authorized for the
        # private S3 bucket (DRIVERS-3628).
        ec2_assume_role(role_arn='${aws_test_secrets_role}'),
        bash_exec(
            command_type=EvgCommandType.SETUP,
            include_expansions_in_env=[
                'build_variant',
                'mongodb_version',
                'AUTH',
                'ORCHESTRATION_FILE',
                'REQUIRE_API_VERSION',
                'TOPOLOGY',
                'AWS_ACCESS_KEY_ID',
                'AWS_SECRET_ACCESS_KEY',
                'AWS_SESSION_TOKEN',
            ],
            script='mongo-cxx-driver/.evergreen/scripts/start-mongod.sh',
        ),
        expansions_update(
            command_type=EvgCommandType.SETUP,
            file='drivers-evergreen-tools/mo-expansion.yml',
        ),
    ]

    @classmethod
    def call(cls, mongodb_version: str, topology: str, vars: Mapping[str, str] = None):
        vars = dict(vars or {})

        vars |= {'mongodb_version': mongodb_version}

        match topology:
            case 'single':
                pass
            case 'replica':
                vars |= {'TOPOLOGY': 'replica_set'}
            case 'sharded':
                vars |= {'TOPOLOGY': 'sharded_cluster'}

        return cls.default_call(vars=vars if vars else None)


def functions():
    return StartMongod().defn()
