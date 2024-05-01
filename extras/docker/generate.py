#!/usr/bin/env python3

from jinja2 import Environment
from jinja2 import FileSystemLoader


MONGOCXX_VERSION = "3.10.1"
MONGOC_VERSION = "1.27.0"
MONGOCRYPT_VERSION = "1.10.0"


def render_template(template_filename, context):
    env = Environment(loader=FileSystemLoader("."))
    template = env.get_template(template_filename)
    rendered = template.render(context)
    return rendered


if __name__ == "__main__":
    templates = [
        ("Makefile.j2", "Makefile"),
        ("TestDockerfile.j2", "TestDockerfile"),
        ("test.cpp.j2", "test.cpp"),
        ("Dockerfile.j2", "Dockerfile"),
    ]
    distros = [
        "alpine3.19",
        "bookworm",
        "noble",
        "redhat-ubi-9.4",
    ]
    base_image = {
        "alpine3.19": "alpine:3.19",
        "bookworm": "debian:12-slim",
        "noble": "ubuntu:22.04",
        "redhat-ubi-9.4": "registry.access.redhat.com/ubi9/ubi-minimal:9.4",
    }
    test_dependency_install_command = {
        "alpine3.19": "apk update && apk upgrade && apk add --no-cache alpine-sdk",
        "bookworm": "apt update && apt upgrade -y && apt install -y build-essential",
        "noble": "apt update && apt upgrade -y && apt install -y build-essential",
        "redhat-ubi-9.4": "microdnf upgrade -y && microdnf install -y g++",
    }
    build_dependencies_install_command = {
        "alpine3.19": "apk update && apk upgrade && apk add --no-cache alpine-sdk ninja cmake openssl-dev python3",
        "bookworm": "apt update && apt upgrade -y && apt install -y ninja-build build-essential cmake git libssl-dev python3 wget",
        "noble": "apt update && apt upgrade -y && apt install -y ninja-build build-essential cmake git libssl-dev python3 wget",
        "redhat-ubi-9.4": "microdnf upgrade -y && microdnf install -y ninja-build gcc g++ git gzip tar wget cmake openssl-devel python3",
    }
    runtime_dependencies_install_command = {
        "alpine3.19": "apk update && apk upgrade && apk add --no-cache openssl3 libstdc++ libc6-compat",
        "bookworm": "apt update && apt upgrade -y && apt install -y libssl3 && rm -rf /var/lib/apt/lists/*",
        "noble": "apt update && apt upgrade -y && apt install -y libssl3 && rm -rf /var/lib/apt/lists/*",
        "redhat-ubi-9.4": "microdnf upgrade -y && microdnf install -y openssl",
    }
    post_install_commands = {
        "alpine3.19": "",
        "bookworm": "\nRUN ldconfig\n",
        "noble": "\nRUN ldconfig\n",
        "redhat-ubi-9.4": (
            "\n"
            "RUN ldconfig\n"
            "\n"
            'ENV LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:/usr/local/lib64/"\n'
        ),
    }
    for template, output_filename in templates:
        if output_filename.endswith(".cpp"):
            comment_token = "//"
        else:
            comment_token = "#"
        header = (
            f"{comment_token} DO NOT EDIT THIS FILE DIRECTLY\n"
            f"{comment_token} This file was auto generated from the template file {template} using the generate.py script\n"
            "\n"
        )
        for distro in distros:
            context = {
                "distro": distro,
                "install_test_dependencies": test_dependency_install_command[distro],
                "install_build_dependencies": build_dependencies_install_command[
                    distro
                ],
                "install_runtime_dependencies": runtime_dependencies_install_command[
                    distro
                ],
                "post_install_commands": post_install_commands[distro],
                "base_image": base_image[distro],
                "mongoc_version": MONGOC_VERSION,
                "mongocxx_version": MONGOCXX_VERSION,
                "mongocrypt_version": MONGOCRYPT_VERSION,
            }

            rendered = render_template(template, context)

            output_path = f"{distro}/{output_filename}"
            with open(output_path, "w") as output_file:
                output_file.write(header)
                output_file.write(rendered)

            print(f"{output_path} generated successfully")
