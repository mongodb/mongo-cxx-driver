/*    Copyright 2014 MongoDB Inc.
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

#include <cstring>

#include "mongo/base/data_view.h"

namespace mongo {

struct ZeroInitTag_t {
    ZeroInitTag_t(){};
};

const ZeroInitTag_t kZeroInitTag;

template <typename Layout, typename ConstView, typename View>
class EncodedValueStorage {
protected:
    EncodedValueStorage() {}

    // This explicit constructor is provided to allow for easy zeroing
    // during creation of a value.  You might prefer this over an
    // uninitialised value if the zeroed version provides a useful base
    // state.  Such cases might include a set of counters that begin at
    // zero, flags that start off false or a larger structure where some
    // significant portion of storage falls into those kind of use cases.
    // Use this where you might have used calloc(1, sizeof(type)) in C.
    //
    // The added value of providing it as a constructor lies in the ability
    // of subclasses to easily inherit a zeroed base state during
    // initialization.
    explicit EncodedValueStorage(ZeroInitTag_t) {
        std::memset(_data, 0, sizeof(_data));
    }

public:
    View view() {
        return _data;
    }

    ConstView constView() const {
        return _data;
    }

    operator View() {
        return view();
    }

    operator ConstView() const {
        return constView();
    }

private:
    char _data[sizeof(Layout)];
};

}  // namespace mongo
