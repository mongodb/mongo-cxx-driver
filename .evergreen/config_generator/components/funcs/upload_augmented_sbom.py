from shrub.v3.evg_command import EvgCommandType, s3_put

from config_generator.etc.function import Function


class UploadAugmentedSBOM(Function):
    name = 'upload augmented sbom'
    commands = [
        s3_put(
            command_type=EvgCommandType.SYSTEM,
            aws_key='${aws_key}',
            aws_secret='${aws_secret}',
            bucket='mciuploads',
            content_type='application/json',
            display_name='Augmented SBOM',
            local_file='mongo-cxx-driver/etc/augmented.sbom.json.new',
            permissions='public-read',
            remote_file='mongo-cxx-driver/${build_variant}/${revision}/${version_id}/${build_id}/silk/augmented.sbom.json',
        ),
        s3_put(
            command_type=EvgCommandType.SYSTEM,
            aws_key='${aws_key}',
            aws_secret='${aws_secret}',
            bucket='mciuploads',
            content_type='application/json',
            display_name='Augmented SBOM (Diff)',
            local_file='mongo-cxx-driver/diff.txt',
            permissions='public-read',
            remote_file='mongo-cxx-driver/${build_variant}/${revision}/${version_id}/${build_id}/silk/augmented.sbom.json.diff',
        ),
    ]

    @classmethod
    def call(cls, **kwargs):
        return cls.default_call(**kwargs)


def functions():
    return UploadAugmentedSBOM.defn()
