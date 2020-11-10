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
    includes = ["."],
    strip_include_prefix = ".",
    include_prefix = "mongocxx/config",
    hdrs = ["config.hpp"],
)

cc_library(
    name = "mongocxx_private_config",
    includes = ["."],
    strip_include_prefix = ".",
    include_prefix = "mongocxx/config",
    hdrs = ["private/config.hh"],
)

cc_library(
    name = "mongocxx_version",
    includes = ["."],
    strip_include_prefix = ".",
    include_prefix = "mongocxx/config",
    hdrs = ["version.hpp"],
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

def _mongocxx_config_implementation(rctx):
    mongocxx_version = "3.6.1"
    mongocxx_version_split = (mongocxx_version + ".").split('.')

    rctx.template(
        "config.hpp",
        rctx.path(rctx.attr.mongocxx_config_template),
        executable = False,
        substitutions = {
            "@MONGOCXX_INLINE_NAMESPACE@": "no_abi",
        },
    )

    rctx.template(
        "private/config.hh",
        rctx.path(rctx.attr.mongocxx_private_config_template),
        executable = False,
        substitutions = {
            "#cmakedefine MONGOCXX_ENABLE_SSL": "#define MONGOCXX_ENABLE_SSL",
        },
    )

    rctx.template(
        "version.hpp",
        rctx.path(rctx.attr.mongocxx_version_template),
        executable = False,
        substitutions = {
            "@MONGOCXX_VERSION@": mongocxx_version,
            "@MONGOCXX_VERSION_MAJOR@": "({})".format(mongocxx_version_split[0]),
            "@MONGOCXX_VERSION_MINOR@": "({})".format(mongocxx_version_split[1]),
            "@MONGOCXX_VERSION_PATCH@": "({})".format(mongocxx_version_split[2]),
            "@MONGOCXX_VERSION_EXTRA@": "({})".format(mongocxx_version_split[3]),
        },
    )

    rctx.file(
        "BUILD.bazel",
        content = _mongocxx_config_build_file_contents,
        executable = False,
    )

_mongocxx_config = repository_rule(
    implementation = _mongocxx_config_implementation,
    attrs = {
        "mongocxx_config_template": attr.label(
            default = Label("@bazelregistry_mongo_cxx_driver//src/mongocxx:config/config.hpp.in"),
            allow_single_file = True,
        ),
        "mongocxx_private_config_template": attr.label(
            default = Label("@bazelregistry_mongo_cxx_driver//src/mongocxx:config/private/config.hh.in"),
            allow_single_file = True,
        ),
        "mongocxx_version_template": attr.label(
            default = Label("@bazelregistry_mongo_cxx_driver//src/mongocxx:config/version.hpp.in"),
            allow_single_file = True,
        ),
    },
)

def mongocxx_config(**kwargs):
    _mongocxx_config(name = "mongocxx_config", **kwargs)
