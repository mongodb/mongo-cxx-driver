from shrub.v3.evg_command import EvgCommandType

from config_generator.etc.function import Function
from config_generator.etc.utils import bash_exec


class RunKMSServers(Function):
    name = 'run_kms_servers'
    commands = [
        bash_exec(
            command_type=EvgCommandType.SETUP,
            script='''\
                set -o errexit
                echo "Preparing CSFLE venv environment..."
                if [[ "${distro_id}" =~ windows-64-vs2015-* ]]; then
                # Python: ImportError: DLL load failed while importing _rust: The specified procedure could not be found.
                echo "Preparing CSFLE venv environment... skipped."
                exit 0
                fi
                cd ./drivers-evergreen-tools/.evergreen/csfle
                # This function ensures future invocations of activate-kmstlsvenv.sh conducted in
                # parallel do not race to setup a venv environment; it has already been prepared.
                # This primarily addresses the situation where the "test" and "run_kms_servers"
                # functions invoke 'activate-kmstlsvenv.sh' simultaneously.
                . ./activate-kmstlsvenv.sh && deactivate
                echo "Preparing CSFLE venv environment... done."
            ''',
        ),
        bash_exec(
            command_type=EvgCommandType.SETUP,
            background=True,
            script='''\
                set -o errexit
                echo "Starting mock KMS servers..."
                if [[ "${distro_id}" =~ windows-64-vs2015-* ]]; then
                # Python: ImportError: DLL load failed while importing _rust: The specified procedure could not be found.
                echo "Starting mock KMS servers... skipped."
                exit 0
                fi
                cd ./drivers-evergreen-tools/.evergreen/csfle
                . ./activate-kmstlsvenv.sh
                python -u kms_http_server.py --ca_file ../x509gen/ca.pem --cert_file ../x509gen/server.pem --port 8999 &
                python -u kms_http_server.py --ca_file ../x509gen/ca.pem --cert_file ../x509gen/expired.pem --port 9000 &
                python -u kms_http_server.py --ca_file ../x509gen/ca.pem --cert_file ../x509gen/wrong-host.pem --port 9001 &
                python -u kms_http_server.py --ca_file ../x509gen/ca.pem --cert_file ../x509gen/server.pem --port 9002 --require_client_cert &
                python -u kms_kmip_server.py &
                echo "Starting mock KMS servers... done."
            ''',
        ),
    ]


def functions():
    return RunKMSServers.defn()
