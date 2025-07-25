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
        9ebfe9f3b51187932ef97270b689da48261acacadd6ea7018d2cc62719c86ffe  uv-aarch64-apple-darwin.tar.gz
        6e0692b817c5d6cfddad13ad177e866e36d95e8d32b4a296a49d937fdcda18d3 *uv-aarch64-pc-windows-msvc.zip
        e82b5a3eb19e5087a6ea92800b0402f60378bd395e3483acd0b46124128ab71f  uv-aarch64-unknown-linux-gnu.tar.gz
        05707835aadb8d0dfacf64a50dc31eb77fb11d1773b13ddd78d2404d43be3b7f  uv-aarch64-unknown-linux-musl.tar.gz
        63f09a167470e40f5a7afce7ba726fa31d700042107215a8f46572a617b2ba64  uv-arm-unknown-linux-musleabihf.tar.gz
        ca3ab5110a248febf3af56ca8e417add5a164228d8861d2f6439d6c77654ad24  uv-armv7-unknown-linux-gnueabihf.tar.gz
        79b9f2f8abf64b93aaf48f87b159bc34e73b92428c6424a9e633f481449bc8be  uv-armv7-unknown-linux-musleabihf.tar.gz
        5d272849a94b7ad36711f336d745e08ed3732042fc51f5c7f28bfc4e95463615 *uv-i686-pc-windows-msvc.zip
        5ca9dcdd8aba4c59266536c00453f3815ccf4852a1bef78f181f4ae43726e742  uv-i686-unknown-linux-gnu.tar.gz
        7a7eb903b673c0a0fa8282e12d2130fb63c1f0e574b1b8eb24ac63503db5db9e  uv-i686-unknown-linux-musl.tar.gz
        ad94079c0aa352730e5656246ecb8fd6cb5c7c1bbcc100954020f32128d6b0de  uv-powerpc64-unknown-linux-gnu.tar.gz
        27ccd00f3c617b59d6977e48612dc5fe49d15c1181bf995863350e78002e54ac  uv-powerpc64le-unknown-linux-gnu.tar.gz
        897257393ca612058bb22691fab3a45480d3f7fc1df5bb968696576d7d98d6c1  uv-riscv64gc-unknown-linux-gnu.tar.gz
        d148aebfc019aa14c686713fb0a391bb700290ecdf2903c2fdab0009182d880b  uv-s390x-unknown-linux-gnu.tar.gz
        77eac9622f76ad89a8c59b31a96277aa61eb290d2949c69ab2061076471aeda2  uv-x86_64-apple-darwin.tar.gz
        4ca84e28b08f48255f95156c5987d61a5e4c51a43372708bc6d84e994eeb7bdb *uv-x86_64-pc-windows-msvc.zip
        427c27ed5f87bf91aa045cf459ea34d348ed6377c62c3c054f1b4046b2f83fe2  uv-x86_64-unknown-linux-gnu.tar.gz
        bfbe34caae328355b720ad0f667be99990b1b93721c8cb726e4a502796eb6d68  uv-x86_64-unknown-linux-musl.tar.gz
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
