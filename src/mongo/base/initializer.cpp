/*    Copyright 2012 10gen Inc.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include "mongo/base/initializer.h"

#include <iostream>
#include "mongo/util/assert_util.h"
#include "mongo/base/global_initializer.h"

namespace mongo {

    Initializer::Initializer() {}
    Initializer::~Initializer() {}

/* This array of function pointers is necessary to force inclusion of
 * translation units with global initializers when linking against a static
 * archive version of the client.  In their absence, the related code may
 * resolve no symbols in a potential output binary, failing to pull in the
 * initializers and leaving the client in a broken state.
 *
 * Working around this with --whole-archive or an equivalent is painful where
 * possible and renders the static archive useless where not possible (on
 * windows).
 *
 * Sorry for the hack...
 */
#define INSTALL_FUNCTION(NAME) void _mongoInitializerFunctionAssure_##NAME();
#include "mongo/base/initializer_functions.h"
#undef INSTALL_FUNCTION

    void (* _mongoGlobalInitializers [])() = {
#define INSTALL_FUNCTION(NAME) &(_mongoInitializerFunctionAssure_##NAME),
#include "mongo/base/initializer_functions.h"
#undef INSTALL_FUNCTION
    NULL
    };

    Status Initializer::execute(const InitializerContext::ArgumentVector& args,
                                const InitializerContext::EnvironmentMap& env) const {

        std::vector<std::string> sortedNodes;
        Status status = _graph.topSort(&sortedNodes);
        if (Status::OK() != status)
            return status;

        InitializerContext context(args, env);

        for (size_t i = 0; i < sortedNodes.size(); ++i) {
            InitializerFunction fn = _graph.getInitializerFunction(sortedNodes[i]);
            if (!fn) {
                return Status(ErrorCodes::InternalError,
                              "topSort returned a node that has no associated function: \"" +
                              sortedNodes[i] + '"');
            }
            try {
                status = fn(&context);
            } catch( const DBException& xcp ) {
                return xcp.toStatus();
            }

            if (Status::OK() != status)
                return status;
        }
        return Status::OK();
    }

    Status runGlobalInitializers(const InitializerContext::ArgumentVector& args,
                                 const InitializerContext::EnvironmentMap& env) {
        return getGlobalInitializer().execute(args, env);
    }

    Status runGlobalInitializers(int argc, const char* const* argv, const char* const* envp) {
        InitializerContext::ArgumentVector args(argc);
        std::copy(argv, argv + argc, args.begin());

        InitializerContext::EnvironmentMap env;

        if (envp) {
            for(; *envp; ++envp) {
                const char* firstEqualSign = strchr(*envp, '=');
                if (!firstEqualSign) {
                    return Status(ErrorCodes::BadValue, "malformed environment block");
                }
                env[std::string(*envp, firstEqualSign)] = std::string(firstEqualSign + 1);
            }
        }

        return runGlobalInitializers(args, env);
    }

    void runGlobalInitializersOrDie(int argc, const char* const* argv, const char* const* envp) {
        Status status = runGlobalInitializers(argc, argv, envp);
        if (!status.isOK()) {
            std::cerr << "Failed global initialization: " << status << std::endl;
            abort();
        }
    }

}  // namespace mongo
