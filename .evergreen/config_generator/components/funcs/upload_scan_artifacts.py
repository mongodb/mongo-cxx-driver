from shrub.v3.evg_command import EvgCommandType, s3_put

from config_generator.etc.function import Function
from config_generator.etc.utils import bash_exec


class UploadScanArtifacts(Function):
    name = 'upload scan artifacts'
    commands = [
        bash_exec(
            command_type=EvgCommandType.TEST,
            working_dir='mongo-cxx-driver',
            script='''\
                set -o errexit
                if find scan -name \\*.html | grep -q html; then
                    (cd scan && find . -name index.html -exec echo "<li><a href='{}'>{}</a></li>" \\;) >> scan.html
                else
                    echo "No issues found" > scan.html
                fi
            '''
        ),
        bash_exec(
            command_type=EvgCommandType.TEST,
            silent=True,
            env={
                'AWS_ACCESS_KEY_ID': '${aws_key}',
                'AWS_SECRET_ACCESS_KEY': '${aws_secret}',
            },
            working_dir='mongo-cxx-driver',
            script=' aws s3 cp scan s3://mciuploads/mongo-cxx-driver/${build_variant}/${revision}/${version_id}/${build_id}/scan/ --recursive --acl public-read --region us-east-1',
        ),
        s3_put(
            command_type=EvgCommandType.SYSTEM,
            aws_key='${aws_key}',
            aws_secret='${aws_secret}',
            bucket='mciuploads',
            content_type='text/html',
            display_name='Scan Build Report',
            local_file='mongo-cxx-driver/scan.html',
            permissions='public-read',
            remote_file='mongo-cxx-driver/${build_variant}/${revision}/${version_id}/${build_id}/scan/index.html',
        ),
    ]

    @classmethod
    def call(cls, **kwargs):
        return cls.default_call(**kwargs)


def functions():
    return UploadScanArtifacts.defn()
