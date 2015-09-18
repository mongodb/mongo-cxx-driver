/**
 *    Copyright (C) 2014 MongoDB Inc.
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

#pragma once

#include <boost/intrusive_ptr.hpp>

#include "mongo/platform/atomic_word.h"

namespace mongo {

class SharedBuffer {
public:
    SharedBuffer() {}

    void swap(SharedBuffer& other) {
        _holder.swap(other._holder);
    }

    /**
     * C++03 compatible way of writing std::move(someSharedBuffer)
     */
    SharedBuffer moveFrom() {
        SharedBuffer out;
        this->swap(out);
        return out;
    }

    static SharedBuffer allocate(size_t bytes) {
        return takeOwnership(static_cast<char*>(malloc(sizeof(Holder) + bytes)));
    }

    /**
     * Given a pointer to a region of un-owned data, prefixed by sufficient space for a
     * SharedBuffer::Holder object, return an SharedBuffer that owns the
     * memory.
     *
     * This class will call free(holderPrefixedData), so it must have been allocated in a way
     * that makes that valid.
     */
    static SharedBuffer takeOwnership(char* holderPrefixedData) {
        // Initialize the refcount to 1 so we don't need to increment it in the constructor
        // (see private Holder* constructor below).
        //
        // TODO: Should dassert alignment of holderPrefixedData
        // here if possible.
        return SharedBuffer(new (holderPrefixedData) Holder(1U));
    }

    char* get() const {
        return _holder ? _holder->data() : NULL;
    }

    class Holder {
    public:
        explicit Holder(AtomicUInt32::WordType initial = AtomicUInt32::WordType())
            : _refCount(initial) {}

        // these are called automatically by boost::intrusive_ptr
        friend void intrusive_ptr_add_ref(Holder* h) {
            h->_refCount.fetchAndAdd(1);
        }

        friend void intrusive_ptr_release(Holder* h) {
            if (h->_refCount.subtractAndFetch(1) == 0) {
                // We placement new'ed a Holder in takeOwnership above,
                // so we must destroy the object here.
                h->~Holder();
                free(h);
            }
        }

        char* data() {
            return reinterpret_cast<char*>(this + 1);
        }

        const char* data() const {
            return reinterpret_cast<const char*>(this + 1);
        }

    private:
        AtomicUInt32 _refCount;
    };

private:
    explicit SharedBuffer(Holder* holder) : _holder(holder, /*add_ref=*/false) {
        // NOTE: The 'false' above is because we have already initialized the Holder with a
        // refcount of '1' in takeOwnership above. This avoids an atomic increment.
    }

    boost::intrusive_ptr<Holder> _holder;
};

inline void swap(SharedBuffer& one, SharedBuffer& two) {
    one.swap(two);
}
}
