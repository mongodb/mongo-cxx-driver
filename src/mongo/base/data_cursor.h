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

#include <cstddef>
#include <cstring>

#include "mongo/base/data_view.h"
#include "mongo/platform/endian.h"

namespace mongo {

class ConstDataCursor : public ConstDataView {
public:
    typedef ConstDataView view_type;

    ConstDataCursor(ConstDataView::bytes_type bytes) : ConstDataView(bytes) {}

    ConstDataCursor operator+(std::size_t s) const {
        return view() + s;
    }

    ConstDataCursor& operator+=(std::size_t s) {
        *this = view() + s;
        return *this;
    }

    ConstDataCursor operator-(std::size_t s) const {
        return view() - s;
    }

    ConstDataCursor& operator-=(std::size_t s) {
        *this = view() - s;
        return *this;
    }

    ConstDataCursor& operator++() {
        return operator+=(1);
    }

    ConstDataCursor operator++(int) {
        ConstDataCursor tmp = *this;
        operator++();
        return tmp;
    }

    ConstDataCursor& operator--() {
        return operator-=(1);
    }

    ConstDataCursor operator--(int) {
        ConstDataCursor tmp = *this;
        operator--();
        return tmp;
    }

    template <typename T>
    ConstDataCursor& skip() {
        *this = view() + sizeof(T);
        return *this;
    }

    template <typename T>
    ConstDataCursor& readNativeAndAdvance(T* t) {
        readNative(t);
        skip<T>();
        return *this;
    }

    template <typename T>
    T readNativeAndAdvance() {
        T out;
        readNativeAndAdvance(&out);
        return out;
    }

    template <typename T>
    T readLEAndAdvance() {
        return endian::littleToNative(readNativeAndAdvance<T>());
    }

    template <typename T>
    T readBEAndAdvance() {
        return endian::bigToNative(readNativeAndAdvance<T>());
    }
};

class DataCursor : public DataView {
public:
    typedef DataView view_type;

    DataCursor(DataView::bytes_type bytes) : DataView(bytes) {}

    operator ConstDataCursor() const {
        return view();
    }

    DataCursor operator+(std::size_t s) const {
        return view() + s;
    }

    DataCursor& operator+=(std::size_t s) {
        *this = view() + s;
        return *this;
    }

    DataCursor operator-(std::size_t s) const {
        return view() - s;
    }

    DataCursor& operator-=(std::size_t s) {
        *this = view() - s;
        return *this;
    }

    DataCursor& operator++() {
        return operator+=(1);
    }

    DataCursor operator++(int) {
        DataCursor tmp = *this;
        operator++();
        return tmp;
    }

    DataCursor& operator--() {
        return operator-=(1);
    }

    DataCursor operator--(int) {
        DataCursor tmp = *this;
        operator--();
        return tmp;
    }

    template <typename T>
    DataCursor& skip() {
        *this = view() + sizeof(T);
        return *this;
    }

    template <typename T>
    DataCursor& readNativeAndAdvance(T* t) {
        readNative(t);
        skip<T>();
        return *this;
    }

    template <typename T>
    T readNativeAndAdvance() {
        T out;
        readNativeAndAdvance(&out);
        return out;
    }

    template <typename T>
    T readLEAndAdvance() {
        return endian::littleToNative(readNativeAndAdvance<T>());
    }

    template <typename T>
    T readBEAndAdvance() {
        return endian::bigToNative(readNativeAndAdvance<T>());
    }

    template <typename T>
    DataCursor& writeNativeAndAdvance(const T& value) {
        writeNative(value);
        skip<T>();
        return *this;
    }

    template <typename T>
    DataCursor& writeLEAndAdvance(const T& value) {
        return writeNativeAndAdvance(endian::nativeToLittle(value));
    }

    template <typename T>
    DataCursor& writeBEAndAdvance(const T& value) {
        return writeNativeAndAdvance(endian::nativeToBig(value));
    }
};

}  // namespace mongo
