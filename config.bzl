_bsoncxx_config_build_file_contents = """
load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "bsoncxx_config",
    includes = ["."],
    strip_include_prefix = ".",
    include_prefix = "bsoncxx/config",
    hdrs = ["config.hpp"],
)

cc_library(
    name = "bsoncxx_version",
    includes = ["."],
    strip_include_prefix = ".",
    include_prefix = "bsoncxx/config",
    hdrs = ["version.hpp"],
)
"""

_mongocxx_config_build_file_contents = """
load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "mongocxx_config",
    hdrs = ["mongoc-config.hpp"],
)

cc_library(
    name = "mongocxx_version",
    hdrs = ["mongoc-version.hpp"],
)
"""

def _bsoncxx_config_implementation(rctx):
    bsoncxx_version = "3.6.1."

    rctx.template(
        "config.hpp",
        rctx.path(rctx.attr.bsoncxx_config_template),
        executable = False,
        substitutions = {
            "#cmakedefine BSONCXX_POLY_USE_STD_EXPERIMENTAL": "",
            "#cmakedefine BSONCXX_POLY_USE_MNMLSTC": "",
            "#cmakedefine BSONCXX_POLY_USE_SYSTEM_MNMLSTC": "",
            "#cmakedefine BSONCXX_POLY_USE_BOOST": "",
            "#cmakedefine BSONCXX_POLY_USE_STD": "#define BSONCXX_POLY_USE_STD 1",
            "@BSONCXX_INLINE_NAMESPACE@": "no_abi",
        },
    )

    rctx.template(
        "version.hpp",
        rctx.path(rctx.attr.bsoncxx_version_template),
        executable = False,
        substitutions = {
            "@BSONCXX_VERSION_MAJOR@": "({})".format(bsoncxx_version.split('.')[0]),
            "@BSONCXX_VERSION_MINOR@": "({})".format(bsoncxx_version.split('.')[1]),
            "@BSONCXX_VERSION_PATCH@": "({})".format(bsoncxx_version.split('.')[2]),
            "@BSONCXX_VERSION_EXTRA@": "({})".format(bsoncxx_version.split('.')[3]),
        },
    )

    rctx.file(
        "BUILD.bazel",
        content = _bsoncxx_config_build_file_contents,
        executable = False,
    )

_bsoncxx_config = repository_rule(
    implementation = _bsoncxx_config_implementation,
    attrs = {
        "bsoncxx_config_template": attr.label(
            default = Label("@bazelregistry_mongo_cxx_driver//src/bsoncxx:config/config.hpp.in"),
            allow_single_file = True,
        ),
        "bsoncxx_version_template": attr.label(
            default = Label("@bazelregistry_mongo_cxx_driver//src/bsoncxx:config/version.hpp.in"),
            allow_single_file = True,
        ),
    },
)

def bsoncxx_config(**kwargs):
    _bsoncxx_config(name = "bsoncxx_config", **kwargs)
