from typing import Literal

from pydantic import BaseModel, validator
from packaging.version import Version


class Distro(BaseModel):
    """
    Defines common properties of a given Evergreen distro.

    * name: Name of the distro.
    * os: Name of the operating system.
    * os_type: One of Linux, MacOS, or Windows.
    * os_ver: Version of the operating system.
    * vs_ver: Version of Visual Studio available.
    * size: Size of tasks the distro is designed to handle.
    * arch: Target architecture.
    """

    name: str
    os: str | None = None
    os_type: Literal['linux', 'macos', 'windows'] | None = None
    os_ver: str | None = None
    vs_ver: Literal[
        '2013',
        '2015',
        '2017',
        '2019',
        '2022',
        'vsCurrent',
        'vsCurrent2',
        'vsMulti',
    ] | None = None
    size: Literal['small', 'large'] | None = None
    arch: Literal['arm64', 'power8', 'zseries'] | None = None

    @validator('os_ver')
    @classmethod
    def validate_os_ver(cls, value):
        return value == 'latest' or Version(value)


def ls_distro(name, **kwargs):
    return [
        Distro(name=f'{name}-large', size='large', **kwargs),
        Distro(name=f'{name}-small', size='small', **kwargs),
    ]

# See: https://evergreen.mongodb.com/distros
# pylint: disable=line-too-long
#fmt: off
DEBIAN_DISTROS = [] + \
    ls_distro(name='debian12-latest', os='debian', os_type='linux', os_ver='latest') + \
    []

MACOS_DISTROS = [
    Distro(name='macos-14', os='macos', os_type='macos', os_ver='14'),
]

MACOS_ARM64_DISTROS = [
    Distro(name='macos-14-arm64', os='macos', os_type='macos', os_ver='14', arch='arm64'),
]

RHEL_DISTROS = [] + \
    ls_distro(name='rhel80', os='rhel', os_type='linux', os_ver='8.0') + \
    ls_distro(name='rhel95', os='rhel', os_type='linux', os_ver='9.5') + \
    ls_distro(name='rhel8-latest', os='rhel', os_type='linux', os_ver='latest') + \
    []

RHEL_ARM64_DISTROS = [] + \
    ls_distro(name='rhel92-arm64', os='rhel', os_type='linux', os_ver='9.2', arch='arm64') + \
    []

RHEL_POWER8_DISTROS = [] + \
    ls_distro(name='rhel8-power', os='rhel', os_type='linux', os_ver='8', arch='power8') + \
    []

RHEL_ZSERIES_DISTROS = [] + \
    ls_distro(name='rhel7-zseries', os='rhel', os_type='linux', os_ver='7', arch='zseries') + \
    ls_distro(name='rhel8-zseries', os='rhel', os_type='linux', os_ver='8', arch='zseries') + \
    []

UBUNTU_DISTROS = [] + \
    ls_distro(name='ubuntu2204', os='ubuntu', os_type='linux', os_ver='22.04') + \
    []

UBUNTU_ARM64_DISTROS = [] + \
    ls_distro(name='ubuntu2004-arm64', os='ubuntu', os_type='linux', os_ver='20.04', arch='arm64') + \
    ls_distro(name='ubuntu2204-arm64', os='ubuntu', os_type='linux', os_ver='22.04', arch='arm64') + \
    []

WINDOWS_DISTROS = [] + \
    ls_distro(name='windows-64-vs2015', os='windows', os_type='windows', vs_ver='2015') + \
    ls_distro(name='windows-vsCurrent', os='windows', os_type='windows', vs_ver='vsCurrent') + \
    []
#fmt: on
# pylint: enable=line-too-long

# Ensure no-arch distros are ordered before arch-specific distros.
ALL_DISTROS = [] + \
    DEBIAN_DISTROS + \
    MACOS_DISTROS + \
    MACOS_ARM64_DISTROS + \
    RHEL_DISTROS + \
    RHEL_ARM64_DISTROS + \
    RHEL_POWER8_DISTROS + \
    RHEL_ZSERIES_DISTROS + \
    UBUNTU_DISTROS + \
    UBUNTU_ARM64_DISTROS + \
    WINDOWS_DISTROS


def find_distro(name) -> Distro:
    candidates = [d for d in ALL_DISTROS if name == d.name]

    if not candidates:
        raise ValueError(f'could not find a distro with the name {name}')

    return candidates[0]


def find_large_distro(name) -> Distro:
    candidates = [d for d in ALL_DISTROS if f'{name}-large' == d.name]

    if candidates:
        return candidates[0]

    return find_distro(name)


def find_small_distro(name) -> Distro:
    candidates = [d for d in ALL_DISTROS if f'{name}-small' == d.name]

    if candidates:
        return candidates[0]

    return find_distro(name)


def make_distro_str(distro_name, compiler, arch) -> str:
    if distro_name.startswith('windows-vsCurrent'):
        # Rename `windows-vsCurrent-*` distros to `windows-<year>` where`<year>`
        # is the Windows Server version used by the distro, e.g.:
        #     ('windows-vsCurrent-2022', 'vs2017x64', None) -> windows-2022-vs2017-x64
        #     ('windows-vsCurrent-2022', 'mingw',     None) -> windows-2022-mingw
        #     ('windows-vsCurrent',      'vs2017x64', None) -> windows-2019-vs2017-x64
        #     ('windows-vsCurrent',      'mingw',     None) -> windows-2019-mingw
        maybe_arch = compiler[len('vs20XY'):]
        if maybe_arch in ('x86', 'x64'):
            compiler_str = compiler[:-len(maybe_arch)] + '-' + maybe_arch
        else:
            compiler_str = compiler
        if distro_name.startswith('windows-vsCurrent-'):
            distro_str = 'windows-' + \
                distro_name[len('windows-vsCurrent-'):] + f'-{compiler_str}'
        else:
            distro_str = 'windows-2019' + f'-{compiler_str}'
    elif distro_name.startswith('windows-64-vs'):
        # Abbreviate 'windows-64-vs<type>' as 'vs<type>' and append '-<arch>' if
        # given in compiler string as 'vs<type><arch>', e.g.:
        #     ('windows-64-vs2017', 'vs2017x64', None) -> vs2017-x64
        #     ('windows-64-vs2017', 'mingw',     None) -> vs2017-mingw
        distro_str = distro_name[len('windows-64-'):] + {
            'vs2013x64': '-x64',
            'vs2013x86': '-x86',
            'vs2015x64': '-x64',
            'vs2015x86': '-x86',
            'vs2017x64': '-x64',
            'vs2017x86': '-x86',
            'vs2019x64': '-x64',
            'vs2019x86': '-x86',
            'vs2022x64': '-x64',
            'vs2022x86': '-x86',
        }.get(compiler, f'-{compiler}')
    else:
        distro_str = distro_name
        if compiler:
            distro_str += f'-{compiler}'

    if arch:
        distro_str += f'-{arch}'

    return distro_str


def to_cc(compiler):
    return {
        'vs2013x64': 'Visual Studio 12 2013',
        'vs2013x86': 'Visual Studio 12 2013',
        'vs2015x64': 'Visual Studio 14 2015',
        'vs2015x86': 'Visual Studio 14 2015',
        'vs2017x64': 'Visual Studio 15 2017',
        'vs2017x86': 'Visual Studio 15 2017',
        'vs2019x64': 'Visual Studio 16 2019',
        'vs2019x86': 'Visual Studio 16 2019',
        'vs2022x64': 'Visual Studio 17 2022',
        'vs2022x86': 'Visual Studio 17 2022',
    }.get(compiler, compiler)


def to_platform(compiler):
    return {
        'vs2013x64': 'x64',
        'vs2013x86': 'Win32',
        'vs2015x64': 'x64',
        'vs2015x86': 'Win32',
        'vs2017x64': 'x64',
        'vs2017x86': 'Win32',
        'vs2019x64': 'x64',
        'vs2019x86': 'Win32',
        'vs2022x64': 'x64',
        'vs2022x86': 'Win32',
    }.get(compiler, compiler)


def compiler_to_vars(compiler):
    match compiler:
        case 'gcc':
            return {
                'cc_compiler': 'gcc',
                'cxx_compiler': 'g++',
            }

        case 'clang':
            return {
                'cc_compiler': 'clang',
                'cxx_compiler': 'clang++',
            }

        case str(vs) if 'vs' in vs:
            return {
                'generator': to_cc(vs),
                'platform': to_platform(vs),
            }

        case _:
            return {}
