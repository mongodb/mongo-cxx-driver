#pragma once

#include <bsoncxx/builder/basic/document.hpp>

#include <mongocxx/options/aggregate.hpp>

namespace mongocxx {
namespace v_noabi {

inline void append_aggregate_options(
    bsoncxx::v_noabi::builder::basic::document& builder,
    options::aggregate const& options) {
    using bsoncxx::v_noabi::builder::basic::kvp;

    if (auto const& allow_disk_use = options.allow_disk_use()) {
        builder.append(kvp("allowDiskUse", *allow_disk_use));
    }

    if (auto const& collation = options.collation()) {
        builder.append(kvp("collation", *collation));
    }

    if (auto const& let = options.let()) {
        builder.append(kvp("let", *let));
    }

    if (auto const& max_time = options.max_time()) {
        builder.append(kvp("maxTimeMS", bsoncxx::v_noabi::types::b_int64{max_time->count()}));
    }

    if (auto const& bypass_document_validation = options.bypass_document_validation()) {
        builder.append(kvp("bypassDocumentValidation", *bypass_document_validation));
    }

    if (auto const& hint = options.hint()) {
        builder.append(kvp("hint", hint->to_value()));
    }

    if (auto const& read_concern = options.read_concern()) {
        builder.append(kvp("readConcern", read_concern->to_document()));
    }

    if (auto const& write_concern = options.write_concern()) {
        builder.append(kvp("writeConcern", write_concern->to_document()));
    }

    if (auto const& batch_size = options.batch_size()) {
        builder.append(kvp("batchSize", *batch_size));
    }

    if (auto const& comment = options.comment()) {
        builder.append(kvp("comment", *comment));
    }
}
} // namespace v_noabi
} // namespace mongocxx
