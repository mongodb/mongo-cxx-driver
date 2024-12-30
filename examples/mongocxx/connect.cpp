// Copyright 2009-present MongoDB, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <cstdlib>
#include <iostream>
#include <string>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/json.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/logger.hpp>
#include <mongocxx/options/client.hpp>
#include <mongocxx/uri.hpp>

#include <examples/macros.hh>

namespace {

class logger final : public mongocxx::logger {
   public:
    explicit logger(std::ostream* stream) : _stream(stream) {}

    void operator()(
        mongocxx::log_level level,
        bsoncxx::stdx::string_view domain,
        bsoncxx::stdx::string_view message) noexcept override {
        if (level >= mongocxx::log_level::k_trace)
            return;
        *_stream << '[' << mongocxx::to_string(level) << '@' << domain << "] " << message << '\n';
    }

   private:
    std::ostream* const _stream;
};

// Use `std::make_unique` with C++14 and newer.
std::unique_ptr<logger> make_logger() {
    return std::unique_ptr<logger>(new logger(&std::cout));
}

} // namespace

int EXAMPLES_CDECL main(int argc, char* argv[]) {
    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_document;

    // The mongocxx::instance constructor and destructor initialize and shut down the driver,
    // respectively. Therefore, a mongocxx::instance must be created before using the driver and
    // must remain alive for as long as the driver is in use.
    mongocxx::instance inst{make_logger()};

    try {
        auto const uri = mongocxx::uri{(argc >= 2) ? argv[1] : mongocxx::uri::k_default_uri};

        mongocxx::options::client client_options;
        if (uri.tls()) {
            mongocxx::options::tls tls_options;
            // NOTE: To test TLS, you may need to set options.
            //
            // If the server certificate is not signed by a well-known CA,
            // you can set a custom CA file with the `ca_file` option.
            // tls_options.ca_file("/path/to/custom/cert.pem");
            //
            // If you want to disable certificate verification, you
            // can set the `allow_invalid_certificates` option.
            // tls_options.allow_invalid_certificates(true);
            client_options.tls_opts(tls_options);
        }

        auto client = mongocxx::client{uri, client_options};

        auto admin = client["admin"];

        auto result = admin.run_command(make_document(kvp("ping", 1)));

        std::cout << bsoncxx::to_json(result) << "\n";

        return EXIT_SUCCESS;

    } catch (std::exception const& xcp) {
        std::cout << "connection failed: " << xcp.what() << "\n";
        return EXIT_FAILURE;
    }
}
