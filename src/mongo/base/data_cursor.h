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

    template <class T>
    struct CursorMethods : public T {

        CursorMethods(typename T::bytes_type bytes) : T(bytes) {}

        T operator+(std::size_t s) const {
            return this->T::view() + s;
        }

        T& operator+=(std::size_t s) {
            *this = this->T::view() + s;
            return *this;
        }

        T operator-(std::size_t s) const {
            return this->T::view() - s;
        }

        T& operator-=(std::size_t s) {
            *this = this->T::view() - s;
            return *this;
        }

        T& operator++() {
            return this->operator+=(1);
        }

        T operator++(int) {
            T tmp = *this;
            operator++();
            return tmp;
        }

        T& operator--() {
            return this->operator-=(1);
        }

        T operator--(int) {
            T tmp = *this;
            operator--();
            return tmp;
        }

        template <typename U>
        void skip() {
            *this = this->T::view() + sizeof(U);
        }

        template <typename U>
        U readNativeAndAdvance() {
            U out = this->T::template readNative<U>();
            this->skip<U>();
            return out;
        }

        template <typename U>
        U readLEAndAdvance() {
            return littleToNative(readNativeAndAdvance<U>());
        }

        template <typename U>
        U readBEAndAdvance() {
            return bigToNative(readNativeAndAdvance<U>());
        }

    };

    class ConstDataCursor : public CursorMethods<ConstDataView> {
    public:

        ConstDataCursor(ConstDataView::bytes_type bytes) : CursorMethods<ConstDataView>(bytes) {}
    };

    class DataCursor : public CursorMethods<DataView> {
    public:

        DataCursor(DataView::bytes_type bytes) : CursorMethods<DataView>(bytes) {}

        template <typename T>
        void writeNativeAndAdvance(const T& value) {
            this->writeNative(value);
            this->skip<T>();
        }

        template <typename T>
        void writeLEAndAdvance(const T& value) {
            return writeNativeAndAdvance(nativeToLittle(value));
        }

        template <typename T>
        void writeBEAndAdvance(const T& value) {
            return writeNativeAndAdvance(nativeToBig(value));
        }

        operator ConstDataCursor() const {
            return view();
        }
    };

} // namespace mongo
