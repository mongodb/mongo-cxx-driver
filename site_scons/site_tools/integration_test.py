"""Pseudo-builders for building and registering unit tests.
"""

def exists(env):
    return True

def build_integration_test(env, target, source, **kwargs):
    libdeps = kwargs.get('LIBDEPS', [])
    libdeps.append( 'integration_test_main' )

    kwargs['LIBDEPS'] = libdeps

    result = env.Program(target, source, **kwargs)
    buildAlias = env.Alias('build-' + target, result)
    env.Alias('integration_tests', buildAlias)
    runAlias = env.Alias('run-' + target, [result],
        "%s --port 27999" % result[0].abspath)
    env.AlwaysBuild(runAlias)
    testAliases = ['integration']
    env.Alias(testAliases, runAlias)
    env.AlwaysBuild(testAliases)

    return result

def generate(env):
    env.AddMethod(build_integration_test, 'IntegrationTest')
