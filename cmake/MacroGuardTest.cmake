# PROJECT_NAME: bsoncxx or mongocxx.
# GUARDED_MACROS: list of macros that should be guarded by prelude/postlude headers.
# INCLUDE_FILTERS: list of regex filters for headers to be added to the tests (must be relative to PROJECT_SOURCE_DIR).
# EXCLUDE_FILTERS: list of regex filters for headers to be excluded (applied after INCLUDE_FILTERS).
# PROJECT_TEST_PROPERTIES: the (bsoncxx|mongocxx)_test_properties target to be linked with.
function(add_macro_guard_test)
    set(opt_args "")
    set(single_args "PROJECT_NAME;PROJECT_TEST_PROPERTIES")
    set(multi_args "GUARDED_MACROS;INCLUDE_FILTERS;EXCLUDE_FILTERS")

    cmake_parse_arguments(PARSED "${opt_args}" "${single_args}" "${multi_args}" ${ARGN})

    if(NOT "${PARSED_UNPARSED_ARGUMENTS}" STREQUAL "")
        message(FATAL_ERROR "unrecognized argument: ${PARSED_UNPARSED_ARGUMENTS}")
    endif()

    foreach(required_arg PROJECT_NAME PROJECT_TEST_PROPERTIES GUARDED_MACROS INCLUDE_FILTERS)
        if("${PARSED_${required_arg}}" STREQUAL "")
            message(FATAL_ERROR "missing required argument: ${required_arg}")
        endif()
    endforeach()

    set(GUARDED_HEADERS "")

    foreach(filter ${PARSED_INCLUDE_FILTERS})
        file(GLOB_RECURSE headers
            LIST_DIRECTORIES false
            RELATIVE ${PROJECT_SOURCE_DIR}
            "${PROJECT_SOURCE_DIR}/${filter}"
        )
        list(APPEND GUARDED_HEADERS ${headers})
    endforeach()

    foreach(filter ${PARSED_EXCLUDE_FILTERS})
        list(FILTER GUARDED_HEADERS EXCLUDE REGEX "${filter}")
    endforeach()

    set(MACRO_GUARD_TEST_PRELUDE "")

    # Check and set initial state.
    foreach(macro ${PARSED_GUARDED_MACROS})
        string(APPEND MACRO_GUARD_TEST_PRELUDE
            "#if defined(${macro})\n"
            "#error \"${macro} is already defined\"\n"
            "#endif\n"
            "#define ${macro} macro guard test\n"
            "\n"
        )
    endforeach()

    # Implement as recursive algorithm for C++11 compatibility.
    string(APPEND MACRO_GUARD_TEST_PRELUDE
        "static constexpr bool compare_equal(const char* lhs, const char* rhs) {\n"
        "    return (*lhs == *rhs) && (*lhs == '\\0' || compare_equal(lhs + 1, rhs + 1));\n"
        "}\n"
        "\n"
        "static_assert(compare_equal(\"abc\", \"abc\"), \"compare_equal() sanity check failed\");\n"
        "static_assert(!compare_equal(\"abc\", \"def\"), \"compare_equal() sanity check failed\");\n"
        "\n"
        "#define _TO_STR(x) #x\n"
        "#define TO_STR(x) _TO_STR(x)\n"
        "\n"
    )

    add_custom_target(test_${PARSED_PROJECT_NAME}_macro_guards)

    # Test each header individually.
    foreach(header ${GUARDED_HEADERS})
        set(MACRO_GUARD_TEST "${MACRO_GUARD_TEST_PRELUDE}")

        # Strip the subdir.
        string(REGEX REPLACE "^(include|lib|test)/(.*)$" "\\1" subdir "${header}")
        string(REGEX REPLACE "^(include|lib|test)/(.*)$" "\\2" header ${header})

        # Apply include prefix to test headers.
        if("${subdir}" STREQUAL "test")
            set(relheader "${PARSED_PROJECT_NAME}/test/${header}")
        else()
            set(relheader "${header}")
        endif()

        # The include directive.
        string(APPEND MACRO_GUARD_TEST "#include <${relheader}>\n\n")

        # Test all guarded macros have been properly restored.
        foreach(macro ${PARSED_GUARDED_MACROS})
            string(APPEND MACRO_GUARD_TEST
                "static_assert(\n"
                "    compare_equal(TO_STR(${macro}),\"macro guard test\"),\n"
                "    \"${macro} was not correctly restored by <${relheader}>\"\n"
                ");\n"
                "\n"
            )
        endforeach()

        # e.g. bsoncxx/v_noabi/bsoncxx/document/view.hpp -> bsoncxx-v_noabi-bsoncxx-document-view
        string(REPLACE "/" "-" test_name "${header}")
        string(REGEX REPLACE "^(.*)\\.(hh|hpp)$" "\\1" test_name "${test_name}")
        configure_file(test_macro_guards.cpp.in macro_guards/${subdir}/${test_name}.cpp)

        # Prefix library name to reduce potential for target conflicts.
        add_library(test-macro_guards-${subdir}-${test_name} STATIC EXCLUDE_FROM_ALL ${CMAKE_CURRENT_BINARY_DIR}/macro_guards/${subdir}/${test_name}.cpp)
        target_link_libraries(test-macro_guards-${subdir}-${test_name} PRIVATE ${PARSED_PROJECT_TEST_PROPERTIES})

        add_dependencies(test_${PARSED_PROJECT_NAME}_macro_guards test-macro_guards-${subdir}-${test_name})
    endforeach()
endfunction()
