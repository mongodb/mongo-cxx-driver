from shrub.v3.evg_build_variant import BuildVariant
from shrub.v3.evg_command import EvgCommandType, KeyValueParam, ec2_assume_role, expansions_update
from shrub.v3.evg_task import EvgTask, EvgTaskRef
from shrub.v3.evg_task_group import EvgTaskGroup

from config_generator.components.funcs.compile import Compile
from config_generator.components.funcs.fetch_det import FetchDET
from config_generator.components.funcs.install_c_driver import InstallCDriver
from config_generator.components.funcs.setup import Setup
from config_generator.etc.distros import find_small_distro
from config_generator.etc.function import Function
from config_generator.etc.utils import bash_exec


class TestOIDC(Function):
    name = 'test-oidc'
    commands = bash_exec(
        command_type=EvgCommandType.TEST,
        working_dir='mongo-cxx-driver',
        script="""\
            export LD_LIBRARY_PATH=$(pwd)/../mongoc/lib

            # Sets environment variables: OIDC_TOKEN_FILE, OIDC_ADMIN_USER, and OIDC_ADMIN_PWD.
            source ${DRIVERS_TOOLS}/.evergreen/auth_oidc/secrets-export.sh
            ./build/src/mongocxx/test/test_driver '[oidc]'
        """,
    )


def functions():
    return TestOIDC.defn()


def tasks():
    updates = [KeyValueParam(key="build_type", value="Debug")]
    icd_vars = {"SKIP_INSTALL_LIBMONGOCRYPT": 1}
    compile_vars = {"ENABLE_TESTS": "ON"}
    commands = [
        expansions_update(updates=updates),
        InstallCDriver.call(vars=icd_vars),
        Compile.call(vars=compile_vars),
        TestOIDC.call(),
    ]

    return [
        EvgTask(
            name="oidc-auth-test-task",
            run_on=[find_small_distro("ubuntu2404").name],
            commands=commands,
        )
    ]


def task_groups():
    return [
        EvgTaskGroup(
            name="test-oidc-task-group",
            tasks=["oidc-auth-test-task"],
            setup_group_can_fail_task=True,
            teardown_group_timeout_secs=180,  # 3 minutes
            setup_group=[
                Setup.call(),
                FetchDET.call(),
                ec2_assume_role(role_arn="${aws_test_secrets_role}"),
                bash_exec(
                    command_type=EvgCommandType.SETUP,
                    include_expansions_in_env=[
                        "AWS_ACCESS_KEY_ID",
                        "AWS_SECRET_ACCESS_KEY",
                        "AWS_SESSION_TOKEN",
                    ],
                    script="./drivers-evergreen-tools/.evergreen/auth_oidc/setup.sh",
                ),
            ],
            teardown_group=[
                bash_exec(
                    command_type=EvgCommandType.SETUP,
                    script="./drivers-evergreen-tools/.evergreen/auth_oidc/teardown.sh",
                )
            ],
        )
    ]


def variants():
    return [
        BuildVariant(
            name="oidc",
            display_name="OIDC",
            tasks=[
                EvgTaskRef(name="test-oidc-task-group"),
            ],
        ),
    ]
