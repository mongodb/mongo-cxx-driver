from config_generator.etc.function import Function
from config_generator.etc.utils import bash_exec

from shrub.v3.evg_command import EvgCommandType, s3_put


class AbiComplianceCheck(Function):
    name = 'abi-compliance-check'
    commands = [
        bash_exec(
            command_type=EvgCommandType.SETUP,
            script='mongo-cxx-driver/.evergreen/scripts/abi-compliance-check-setup.sh'
        ),
        bash_exec(
            command_type=EvgCommandType.TEST,
            script='mongo-cxx-driver/.evergreen/scripts/abi-compliance-check-test.sh'
        ),
        s3_put(
            command_type=EvgCommandType.SYSTEM,
            aws_key='${aws_key}',
            aws_secret='${aws_secret}',
            bucket='mciuploads',
            content_type='text/html',
            display_name='ABI Compliance Check (Stable): ',
            local_files_include_filter='cxx-abi/compat_reports/**/compat_report.html',
            permissions='public-read',
            remote_file='mongo-cxx-driver/${branch_name}/${revision}/${version_id}/${build_id}/${execution}/abi-compliance-check/abi/',
        ),
        s3_put(
            command_type=EvgCommandType.SYSTEM,
            aws_key='${aws_key}',
            aws_secret='${aws_secret}',
            bucket='mciuploads',
            content_type='text/plain',
            display_name='ABI Compliance Check (Stable): ',
            local_files_include_filter='cxx-abi/logs/**/log.txt',
            permissions='public-read',
            remote_file='mongo-cxx-driver/${branch_name}/${revision}/${version_id}/${build_id}/${execution}/abi-compliance-check/abi/',
        ),
        s3_put(
            command_type=EvgCommandType.SYSTEM,
            aws_key='${aws_key}',
            aws_secret='${aws_secret}',
            bucket='mciuploads',
            content_type='text/html',
            display_name='ABI Compliance Check (Unstable): ',
            local_files_include_filter='cxx-noabi/compat_reports/**/compat_report.html',
            permissions='public-read',
            remote_file='mongo-cxx-driver/${branch_name}/${revision}/${version_id}/${build_id}/${execution}/abi-compliance-check/noabi/',
        ),
        s3_put(
            command_type=EvgCommandType.SYSTEM,
            aws_key='${aws_key}',
            aws_secret='${aws_secret}',
            bucket='mciuploads',
            content_type='text/plain',
            display_name='ABI Compliance Check (Unstable): ',
            local_files_include_filter='cxx-noabi/logs/**/log.txt',
            permissions='public-read',
            remote_file='mongo-cxx-driver/${branch_name}/${revision}/${version_id}/${build_id}/${execution}/abi-compliance-check/noabi/',
        ),
    ]

    @classmethod
    def call(cls, **kwargs):
        return cls.default_call(**kwargs)


def functions():
    return AbiComplianceCheck.defn()
