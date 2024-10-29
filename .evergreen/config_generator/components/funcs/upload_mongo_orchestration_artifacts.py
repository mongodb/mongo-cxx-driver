from shrub.v3.evg_command import EvgCommandType, s3_put

from config_generator.etc.function import Function
from config_generator.etc.utils import bash_exec


class UploadMongoOrchestrationArtifacts(Function):
    name = 'upload mongo orchestration artifacts'
    commands = [
        bash_exec(
            command_type=EvgCommandType.SYSTEM,
            script='''\
                set -o errexit
                for log in $(find . -name '*.log'); do
                  tar rf mongodb-logs.tar "$log"
                done
                if [[ -f mongodb-logs.tar ]]; then
                  gzip mongodb-logs.tar
                fi
            '''
        ),
        s3_put(
            command_type=EvgCommandType.SYSTEM,
            aws_key='${aws_key}',
            aws_secret='${aws_secret}',
            bucket='mciuploads',
            content_type='${content_type|application/x-gzip}',
            display_name='mongodb-logs.tar.gz',
            local_file='mongodb-logs.tar.gz',
            optional=True,
            permissions='public-read',
            remote_file='mongo-cxx-driver/${build_variant}/${revision}/${version_id}/${build_id}/logs/${task_id}-${execution}-mongodb-logs.tar.gz',
        ),
    ]

    @classmethod
    def call(cls, **kwargs):
        return cls.default_call(**kwargs)


def functions():
    return UploadMongoOrchestrationArtifacts.defn()
