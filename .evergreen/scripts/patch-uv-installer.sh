#!/usr/bin/env bash

# A convenient helper function to download the list of checksums for the specified release.
# The output of this function should be copy-pasted as-is into the array of checksums below.
download_checksums() {
  declare version
  version="${1:?"usage: download_checkums <version>"}"

  for checksum in $(curl -sSL "https://github.com/astral-sh/uv/releases/download/${version:?}/dist-manifest.json" | jq -r '.releases[0].artifacts.[] | select(startswith("uv-") and (endswith(".zip.sha256") or endswith(".tar.gz.sha256")))'); do
    curl -sSL "https://github.com/astral-sh/uv/releases/download/${version:?}/${checksum:?}"
  done
}

# Patches the specified uv-installer.sh script with checksums.
patch_uv_installer() {
    declare script version
    script="${1:?"usage: patch_uv_installer <path/to/uv-installer.sh> <version>"}"
    version="${2:?"usage: patch_uv_installer <path/to/uv-installer.sh> <version>"}"

    [[ -f "${script:?}" ]] || {
        echo "${script:?} does not exist?"
        return 1
    } >&2

    command -v perl >/dev/null || return

    # Ensure the uv-installer.sh script's version matches the expected version.
    app_version="$(perl -lne 'print $1 if m|APP_VERSION="([^"]+)"|' "${script:?}")" || return

    [[ "${app_version:?}" == "${version:?}" ]] || {
        echo "${script:?} version ${app_version:?} does not match expected version ${version:?}"
        return 1
    } >&2

    # The output of the `download_checksums` helper function.
    checksums=(
        15269226c753f01137b5a35c79e59ab46d8aab25a242641fdc4003a6d0a831ff  uv-aarch64-apple-darwin.tar.gz
        a2891f1f1c56e717115579da655951007e2e5e498535b473d9f7cbffe7369e1a *uv-aarch64-pc-windows-msvc.zip
        6fd314ca589788265ff99ec754bd2fa2a5d090ef592ddbbe8ded6b141615a491  uv-aarch64-unknown-linux-gnu.tar.gz
        d78076c7e0dfcd3580736c11e009ef856bd13015f00406e3ded1fc895e2104ba  uv-aarch64-unknown-linux-musl.tar.gz
        87d33a579cc2ee986e3163bb5de1791c6052ef8716d5fb8d8bf73aa2e5f1bf66  uv-arm-unknown-linux-musleabihf.tar.gz
        4139d74dfe3192bc71dacd3cf0cdf6a006d121db2c4d841e08f369761baaf00e  uv-armv7-unknown-linux-gnueabihf.tar.gz
        5d4d117cebddc7dbdbf6e1410c65868f113a75700acac77eed28dfdc0411c113  uv-armv7-unknown-linux-musleabihf.tar.gz
        4eaa185b61f9cfe73ab7534de7282e51cc0f6bf47361429bdcbb6a5f3264f6e4 *uv-i686-pc-windows-msvc.zip
        7fb9324cfb0c57a9b9145e73598794c7bc4df01f50730bd926d4ab10d4fc59ff  uv-i686-unknown-linux-gnu.tar.gz
        3c2e2d69e8da093df76ffffa9758669c33ae47624f73d06ec2a83a363f17fbd4  uv-i686-unknown-linux-musl.tar.gz
        264dbfddd58cdbd35b33ea24dd802a3409eae1d4516d057bdff88df2e257eaa2  uv-powerpc64-unknown-linux-gnu.tar.gz
        d34c6d7df2ed9e9739bc0313875e602a943b17fccbf1127f824a5ff9a3253bb5  uv-powerpc64le-unknown-linux-gnu.tar.gz
        6bcd5a72977a4362f042cc2568960b5a42c1f13240ada6b1cce6aa2f6d6a3e42  uv-riscv64gc-unknown-linux-gnu.tar.gz
        8b4372280249038ea5824937d6e1ca7e2192061bd38f3362200133ff55cbb9c3  uv-s390x-unknown-linux-gnu.tar.gz
        4b1da363d8913a85a4a40df6620ae800b16c62beb54f60b1d336432644cb12bb  uv-x86_64-apple-darwin.tar.gz
        f7ed402ea1e4d7fb2b2490e1a097e9849bfdaaa689521d290bdce5478db0428f *uv-x86_64-pc-windows-msvc.zip
        5429c9b96cab65198c2e5bfe83e933329aa16303a0369d5beedc71785a4a2f36  uv-x86_64-unknown-linux-gnu.tar.gz
        0d89cffae3ad1c4ae2d4da06f71ad4539974185a31f7c196a6151b400bf84039  uv-x86_64-unknown-linux-musl.tar.gz
    )

    # Substitution:
    #     local _checksum_value
    # ->
    #     local _checksum_value="sha256"
    perl -i'' -lpe "s|local _checksum_style$|local _checksum_style=\"sha256\"|" "${script:?}" || return

    # Substitution (for each checksum + artifact in the checksums array):
    #     case "$_artifact_name" in
    #         ...
    #         "<artifact>")
    #         ...
    #     esac
    # ->
    #     case "$_artifact_name" in
    #         ...
    #         "<artifact>") _checksum_value="<checksum>"
    #         ...
    #     esac
    for ((i=0; i<"${#checksums[@]}"; i+=2)); do
        declare checksum artifact
        checksum="${checksums[i]:?}"
        artifact="${checksums[i+1]:?}"

        [[ "${artifact:?}" =~ ^\* ]] && artifact="${artifact:1}"
        perl -i'' -lpe "s|(\"${artifact:?}\"\))|\$1 _checksum_value=\"${checksum:?}\"|" "${script:?}" || return
    done
}
