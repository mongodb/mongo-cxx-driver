from shrub.v3.evg_command import EvgCommandType, archive_targz_pack, s3_put

from config_generator.etc.function import Function


class UploadWorkingDir(Function):
    name = 'upload working dir'
    commands = [
        archive_targz_pack(
            command_type=EvgCommandType.SYSTEM,
            include=['./**'],
            source_dir='mongo-cxx-driver',
            target='working-dir.tar.gz',
        ),
        s3_put(
            command_type=EvgCommandType.SYSTEM,
            aws_key='${aws_key}',
            aws_secret='${aws_secret}',
            bucket='mciuploads',
            content_type='${content_type|application/x-gzip}',
            display_name='working-dir.tar.gz',
            local_file='working-dir.tar.gz',
            permissions='public-read',
            remote_file='mongo-cxx-driver/${build_variant}/${revision}/${version_id}/${build_id}/artifacts/${task_id}-${execution}-working-dir.tar.gz',
        ),
    ]


def functions():
    return UploadWorkingDir.defn()
