from shrub.v3.evg_command import EvgCommandType, ec2_assume_role

from config_generator.etc.function import Function
from config_generator.etc.utils import bash_exec


class CSFLESetup(Function):
    name = 'csfle-setup'
    commands = [
        bash_exec(
            command_type=EvgCommandType.SETUP,
            script="""\
                set -o errexit
                echo "Preparing CSFLE venv environment..."
                cd ./drivers-evergreen-tools/.evergreen/csfle
                # This function ensures future invocations of activate-kmstlsvenv.sh conducted in
                # parallel do not race to setup a venv environment; it has already been prepared.
                # This primarily addresses the situation where the "test" and "csfle-setup"
                # functions invoke 'activate-kmstlsvenv.sh' simultaneously.
                . ./activate-kmstlsvenv.sh && deactivate
                echo "Preparing CSFLE venv environment... done."
            """,
        ),
        ec2_assume_role(role_arn='${aws_test_secrets_role}'),
        bash_exec(
            command_type=EvgCommandType.SETUP,
            working_dir='drivers-evergreen-tools/.evergreen/csfle',
            include_expansions_in_env=['AWS_ACCESS_KEY_ID', 'AWS_SECRET_ACCESS_KEY', 'AWS_SESSION_TOKEN'],
            # Creates secrets-export.sh. Starts servers on ports 5698, 9000, 9001, 9002, and 9003.
            script='./setup.sh',
        ),
    ]


def functions():
    return CSFLESetup.defn()
