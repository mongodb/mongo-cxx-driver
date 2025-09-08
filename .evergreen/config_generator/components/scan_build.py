from shrub.v3.evg_build_variant import BuildVariant
from shrub.v3.evg_command import EvgCommandType, s3_put
from shrub.v3.evg_task import EvgTask, EvgTaskRef

from config_generator.components.funcs.fetch_c_driver_source import FetchCDriverSource
from config_generator.components.funcs.install_uv import InstallUV
from config_generator.components.funcs.setup import Setup
from config_generator.etc.distros import find_large_distro
from config_generator.etc.function import Function, merge_defns
from config_generator.etc.utils import bash_exec

TAG = 'scan-build'


# pylint: disable=line-too-long
# fmt: off
MATRIX = [
    (11,  None    ),
    (11, 'impls'  ),

    (14,  None    ),
    (14, 'impls'  ),

    (17,  None    ),
    (17, 'impls'  ),
]
# fmt: on
# pylint: enable=line-too-long


class RunScanBuild(Function):
    name = 'run scan build'
    commands = [
        bash_exec(
            command_type=EvgCommandType.TEST,
            add_expansions_to_env=True,
            redirect_standard_error_to_output=True,
            working_dir='mongo-cxx-driver',
            script='.evergreen/scripts/compile-scan-build.sh',
        ),
    ]

    @classmethod
    def call(cls, cxx_standard, polyfill):
        vars = {'CXX_STANDARD': cxx_standard}

        if polyfill is not None:
            vars |= {'BSONCXX_POLYFILL': polyfill}

        return cls.default_call(vars=vars)


class UploadScanArtifacts(Function):
    name = 'upload scan artifacts'
    commands = [
        bash_exec(
            command_type=EvgCommandType.TEST,
            working_dir='mongo-cxx-driver',
            script="""\
                set -o errexit
                if find scan -name \\*.html | grep -q html; then
                    (cd scan && find . -name index.html -exec echo "<li><a href='{}'>{}</a></li>" \\;) >> scan.html
                else
                    echo "No issues found" > scan.html
                fi
            """,
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


def functions():
    return merge_defns(
        RunScanBuild.defn(),
        UploadScanArtifacts.defn(),
    )


def tasks():
    res = []

    distro_name = 'rhel80'
    distro = find_large_distro(distro_name)

    for cxx_standard, polyfill in MATRIX:
        name = f'{TAG}-{distro_name}-std{cxx_standard}'
        tags = [TAG, distro_name, f'std{cxx_standard}']

        if polyfill is not None:
            name += f'-{polyfill}'
            tags.append(polyfill)
        else:
            name += '-default'

        res.append(
            EvgTask(
                name=name,
                tags=tags,
                run_on=distro.name,
                commands=[
                    Setup.call(),
                    FetchCDriverSource.call(),
                    InstallUV.call(),
                    RunScanBuild.call(cxx_standard, polyfill),
                    UploadScanArtifacts.call(),
                ],
            )
        )

    return res


def variants():
    return [
        BuildVariant(
            name=f'{TAG}-matrix',
            display_name=f'{TAG}-matrix',
            tasks=[EvgTaskRef(name=f'.{TAG}')],
        ),
    ]
