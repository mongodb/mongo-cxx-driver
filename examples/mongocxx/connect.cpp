// Copyright 2015 MongoDB Inc.
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

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/stdx/make_unique.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/logger.hpp>
#include <mongocxx/options/client.hpp>
#include <mongocxx/uri.hpp>

namespace {

class logger final : public mongocxx::logger {
   public:
    explicit logger(std::ostream* stream) : _stream(stream) {
    }

    void operator()(mongocxx::log_level level, mongocxx::stdx::string_view domain,
                    mongocxx::stdx::string_view message) noexcept override {
        if (level >= mongocxx::log_level::k_trace) return;
        *_stream << '[' << mongocxx::to_string(level) << '@' << domain << "] " << message << '\n';
    }

   private:
    std::ostream* const _stream;
};

}  // namespace

int main(int argc, char* argv[]) {
    using bsoncxx::builder::stream::document;

    mongocxx::instance inst{bsoncxx::stdx::make_unique<logger>(&std::cout)};

    try {
        const auto uri = mongocxx::uri{(argc >= 2) ? argv[1] : mongocxx::uri::k_default_uri};

        mongocxx::options::client client_options;
        if (uri.ssl()) {
            mongocxx::options::ssl ssl_options;
            // NOTE: To test SSL, you may need to set options. The following
            // would enable certificates for Homebrew OpenSSL on OS X.
            // options.ca_file("/usr/local/etc/openssl/cert.pem");
            // ssl_options.ca_file("/usr/local/etc/openssl/cert.pem");
            client_options.ssl_opts(ssl_options);
        }

        auto client = mongocxx::client{uri, client_options};

        auto admin = client["admin"];

        document ismaster;
        ismaster << "isMaster" << 1;

        auto result = admin.run_command(ismaster.view());

        std::cout << bsoncxx::to_json(result) << "\n";

        return EXIT_SUCCESS;

    } catch (const std::exception& xcp) {
        std::cout << "connection failed: " << xcp.what() << "\n";
        return EXIT_FAILURE;
    }
}
