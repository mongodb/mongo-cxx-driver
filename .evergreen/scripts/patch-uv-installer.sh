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
        954d24634d5f37fa26c7af75eb79893d11623fc81b4de4b82d60d1ade4bfca22  uv-aarch64-apple-darwin.tar.gz
        0e1e80ca0d6faab2c716ac7b73214c6df7a37dc0a45998d5f7d6e73339db8e48 *uv-aarch64-pc-windows-msvc.zip
        27da35ef54e9131c2e305de67dd59a07c19257882c6b1f3cf4d8d5fbb8eaf4ca  uv-aarch64-unknown-linux-gnu.tar.gz
        725237095ea91ad4fe54b1a047138194bdef428002709e0ae9dfea33b66e1c8a  uv-aarch64-unknown-linux-musl.tar.gz
        ea523b34d5c96b32f1f68a51b690774f7822474a355fd74973a089ba2002e82a  uv-arm-unknown-linux-musleabihf.tar.gz
        8fcbdce85af3b62b8625e780c0065ccd35aab830fcd380c2bb5a03e2174861b0  uv-armv7-unknown-linux-gnueabihf.tar.gz
        d433a35cd4e0b9ed8163b42b69d050954b77163765e5c25ac21aa08da2117ad3  uv-armv7-unknown-linux-musleabihf.tar.gz
        d862eaf046abe9143b3e172137cb50657920d324cb3e00be78d22aba4478d7f8 *uv-i686-pc-windows-msvc.zip
        fc0baebac65fba4865c23f0ec31722750543c5f5320d7c52221b0565065520ce  uv-i686-unknown-linux-gnu.tar.gz
        fff6ad20fbcf808089823161a303ab32bfbec31b0af73c3d5717fe8b80aa8a81  uv-i686-unknown-linux-musl.tar.gz
        d0cfde8f85eeeb7baabbca6ad7e66fba0bfb2e6145a95fb9fc97d82d153d08c8  uv-powerpc64-unknown-linux-gnu.tar.gz
        f44b3870a323ba5dbd189c46ef10e565f9ba94c1f675b1daf8d1deab7cda92fb  uv-powerpc64le-unknown-linux-gnu.tar.gz
        87e97d546129846a2ee73516fa71770ee787ee62121ebfc5c3db682ae0f54cf1  uv-riscv64gc-unknown-linux-gnu.tar.gz
        de324cd0b04162e25fd1dc2332b05e556978fec4bfcc3fbea6565e8276caac53  uv-s390x-unknown-linux-gnu.tar.gz
        ae755df53c8c2c1f3dfbee6e3d2e00be0dfbc9c9b4bdffdb040b96f43678b7ce  uv-x86_64-apple-darwin.tar.gz
        503a430df36957f851daedaed707a2ab69ed21a48bd499c2f0f7253ad5ba983f *uv-x86_64-pc-windows-msvc.zip
        6dcb28a541868a455aefb2e8d4a1283dd6bf888605a2db710f0530cec888b0ad  uv-x86_64-unknown-linux-gnu.tar.gz
        2df1c90246ac03b237f997dce0a31a757523a6aaa9a15b26996a9851dc08ad3c  uv-x86_64-unknown-linux-musl.tar.gz
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
