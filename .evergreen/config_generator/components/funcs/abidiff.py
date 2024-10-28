from config_generator.etc.function import Function
from config_generator.etc.utils import bash_exec

from shrub.v3.evg_command import EvgCommandType, s3_put


class Abidiff(Function):
    name = 'abidiff'
    commands = [
        bash_exec(
            command_type=EvgCommandType.SETUP,
            script='mongo-cxx-driver/.evergreen/scripts/abidiff-setup.sh'
        ),
        bash_exec(
            command_type=EvgCommandType.TEST,
            script='mongo-cxx-driver/.evergreen/scripts/abidiff-test.sh'
        ),
        s3_put(
            command_type=EvgCommandType.SYSTEM,
            aws_key='${aws_key}',
            aws_secret='${aws_secret}',
            bucket='mciuploads',
            content_type='text/plain',
            display_name='abidiff (Stable): ',
            local_files_include_filter='cxx-abi/*.txt',
            permissions='public-read',
            remote_file='mongo-cxx-driver/${branch_name}/${revision}/${version_id}/${build_id}/${execution}/abidiff/abi/',
        ),
        s3_put(
            command_type=EvgCommandType.SYSTEM,
            aws_key='${aws_key}',
            aws_secret='${aws_secret}',
            bucket='mciuploads',
            content_type='text/plain',
            display_name='abidiff (Unstable): ',
            local_files_include_filter='cxx-noabi/*.txt',
            permissions='public-read',
            remote_file='mongo-cxx-driver/${branch_name}/${revision}/${version_id}/${build_id}/${execution}/abidiff/noabi/',
        ),
    ]

    @classmethod
    def call(cls, **kwargs):
        return cls.default_call(**kwargs)


def functions():
    return Abidiff.defn()
