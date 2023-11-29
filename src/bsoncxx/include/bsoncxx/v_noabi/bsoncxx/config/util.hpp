// clang-format off
/**
 * @brief Convert the given macro argument to a string literal, after macro expansion
 */
#define bsoncxx_stringify(...) bsoncxx_stringify_impl(__VA_ARGS__)
#define bsoncxx_stringify_impl(...) #__VA_ARGS__

/**
 * @brief Token-paste two macro arguments, after macro expansion
 */
#define bsoncxx_concat(A, ...) bsoncxx_concat_impl(A, __VA_ARGS__)
#define bsoncxx_concat_impl(A, ...) A##__VA_ARGS__

/**
 * @brief Expands to a _Pragma() preprocessor directive, after macro expansion
 *
 * The arguments an arbitrary "token soup", and should not be quoted like a regular
 * _Pragma. This macro will stringify-them itself.
 *
 * Example:
 *
 *      bsoncxx_pragma(GCC diagnostic ignore "-Wconversion")
 *
 * will become:
 *
 *      _Pragma("GCC diagnostic ignore \"-Wconversion\"")
 *
 */
#define bsoncxx_pragma(...) _bsoncxxPragma(__VA_ARGS__)
#ifdef _MSC_VER
// Old MSVC doesn't recognize C++11 _Pragma(), but it always recognized __pragma
#define _bsoncxxPragma(...) __pragma(__VA_ARGS__)
#else
#define _bsoncxxPragma(...) _Pragma(bsoncxx_stringify(__VA_ARGS__))
#endif

/**
 * @brief Use in a declaration position to force the appearence of a semicolon
 * as the next token. Use this for statement-like or declaration-like macros to
 * enforce that their call sites are followed by a semicolon
 */
#define bsoncxx_force_semicolon static_assert(true, "")

/**
 * @brief Add a trailing noexcept, decltype-return, and return-body to a
 * function definition. (Not compatible with lambda expressions.)
 *
 * Example:
 *
 *      template <typename T>
 *      auto foo(T x, T y) bsoncxx_returns(x + y);
 *
 * Becomes:
 *
 *      template <typename T>
 *      auto foo(T x, T y) noexcept(noexcept(x + y))
 *          -> decltype(x + y)
 *      { return x + y };
 *
 */
#define bsoncxx_returns(...)                                 \
    noexcept(noexcept(__VA_ARGS__))->decltype(__VA_ARGS__) { \
        return __VA_ARGS__;                                  \
    }                                                        \
    bsoncxx_force_semicolon

/**
 * @macro mongocxx_cxx14_constexpr
 * @brief Expands to `constexpr` if compiling as c++14 or greater, otherwise
 * expands to `inline`.
 *
 * Use this on functions that can only be constexpr in C++14 or newer, including
 * non-const member functions.
 */
#if __cplusplus >= 201402L || (defined(_MSVC_LANG) && _MSVC_LANG >= 201402L)
#define bsoncxx_cxx14_constexpr constexpr
#else
#define bsoncxx_cxx14_constexpr inline
#endif

/**
 * @brief Disable a warning for a particular compiler.
 *
 * The argument should be of the form:
 *
 * - Clang(<flag-string-literal>)
 * - GCC(<flag-string-literal>)
 * - GNU(<flag-string-literal>)
 * - MSVC(<id-integer-literal>)
 *
 * The "GNU" form applies to both GCC and Clang
 */
#define bsoncxx_disable_warning(Spec) \
    bsoncxx_concat(_bsoncxxDisableWarningImpl_for_, Spec) \
    bsoncxx_force_semicolon

/**
 * @brief Push the current compiler diagnostics settings state
 */
#define bsoncxx_push_warnings() \
    bsoncxx_if_gnu_like(bsoncxx_pragma(GCC diagnostic push)) \
    bsoncxx_if_msvc(bsoncxx_pragma(warning(push))) \
    bsoncxx_force_semicolon

/**
 * @brief Restore prior compiler diagnostics settings from before the most
 * recent bsoncxx_push_warnings()
 */
#define bsoncxx_pop_warnings() \
    bsoncxx_if_gnu_like(bsoncxx_pragma(GCC diagnostic pop)) \
    bsoncxx_if_msvc(bsoncxx_pragma(warning(pop))) \
    bsoncxx_force_semicolon

#define _bsoncxxDisableWarningImpl_for_GCC(...) \
    bsoncxx_if_gcc(bsoncxx_pragma(GCC diagnostic ignored __VA_ARGS__))

#define _bsoncxxDisableWarningImpl_for_Clang(...) \
    bsoncxx_if_clang(bsoncxx_pragma(GCC diagnostic ignored __VA_ARGS__))

#define _bsoncxxDisableWarningImpl_for_GNU(...) \
    _bsoncxxDisableWarningImpl_for_GCC(__VA_ARGS__) \
    _bsoncxxDisableWarningImpl_for_Clang(__VA_ARGS__)

#define _bsoncxxDisableWarningImpl_for_MSVC(...) \
    bsoncxx_if_msvc(bsoncxx_pragma(warning(disable : __VA_ARGS__)))
