#!/usr/bin/env python3

from jinja2 import Environment
from jinja2 import FileSystemLoader


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
    ]
    distros = [
        "alpine3.18",
        "bookworm",
        "redhat-ubi-9.2",
    ]
    test_dependency_install_command = {
        "alpine3.18": "apk add alpine-sdk",
        "bookworm": "apt update && apt install -y build-essential",
        "redhat-ubi-9.2": "microdnf install -y g++",
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
            }

            rendered = render_template(template, context)

            output_path = f"{distro}/{output_filename}"
            with open(output_path, "w") as output_file:
                output_file.write(header)
                output_file.write(rendered)

            print(f"{output_path} generated successfully")
