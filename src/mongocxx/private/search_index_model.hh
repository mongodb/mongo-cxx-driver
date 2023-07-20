#pragma once

#include <bsoncxx/document/view_or_value.hpp>
#include <mongocxx/private/client_session.hh>
#include <mongocxx/private/libbson.hh>
#include <mongocxx/private/libmongoc.hh>

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

class search_index_model::impl {
   public:
    impl(bsoncxx::document::view_or_value definition) : _definition(definition.view()) {}
    impl(bsoncxx::string::view_or_value name, bsoncxx::document::view_or_value definition)
        : _name(name), _definition(definition.view()) {}

    ~impl() = default;

    impl(impl&& i) = delete;

    impl& operator=(impl&& i) = delete;

    impl(const impl& i) : _name(i._name), _definition(i._definition) {}

    impl& operator=(const impl& i) {
        _name = i._name;
        _definition = i._definition;
        return *this;
    }

    bsoncxx::stdx::optional<bsoncxx::string::view_or_value> name() const {
        return _name;
    }

    bsoncxx::document::view definition() const {
        return _definition.view();
    }

   private:
    bsoncxx::stdx::optional<bsoncxx::string::view_or_value> _name;
    bsoncxx::document::view_or_value _definition;
};

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx