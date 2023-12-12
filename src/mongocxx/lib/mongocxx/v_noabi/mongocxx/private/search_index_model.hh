#pragma once

#include <bsoncxx/document/view_or_value.hpp>
#include <mongocxx/search_index_model.hpp>

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
namespace v_noabi {

class search_index_model::impl {
   public:
    impl(bsoncxx::document::view_or_value definition) : _definition(definition.view()) {}
    impl(bsoncxx::string::view_or_value name, bsoncxx::document::view_or_value definition)
        : _name(name), _definition(definition.view()) {}

    bsoncxx::stdx::optional<bsoncxx::string::view_or_value> _name;
    bsoncxx::document::view_or_value _definition;
};

}  // namespace v_noabi
}  // namespace mongocxx

#include <mongocxx/config/private/postlude.hh>
