"""Pseudo-builders for building and registering unit tests.
"""

def exists(env):
    return True

def build_cpp_unit_test(env, target, source, **kwargs):
    result = env.Program(target, source, **kwargs)
    buildAlias = env.Alias('build-' + target, result)
    env.Alias('unittests', buildAlias)
    env['ENV']['GTEST_FILTER'] = env.get("gtest_filter", "*")
    runAlias = env.Alias('run-' + target, [result], result[0].abspath)
    env.AlwaysBuild(runAlias)
    testAliases = ['test', 'smokeCppUnittests', 'smoke']
    env.Alias(testAliases, runAlias)
    env.AlwaysBuild(testAliases)

    return result

def generate(env):
    env.AddMethod(build_cpp_unit_test, 'CppUnitTest')
