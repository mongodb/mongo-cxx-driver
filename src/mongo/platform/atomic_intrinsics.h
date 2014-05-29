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

/**
 * NOTE: Not for direct use by any code other than AtomicWord.
 *
 * The atomic_intrinsics module provides low-level atomic operations for manipulating memory.
 * Implementations are platform specific, so this file describes the interface and includes
 * the appropriate os/compiler-specific headers.
 *
 * For supported word types, the atomic_intrinsics headers provide implementations of template
 * classes of the following form:
 *
 * template <typename T> class AtomicIntrinsics {
 *     static T load(volatile const T* value);
 *     static T store(volatile T* dest, T newValue);
 *     static T compareAndSwap(volatile T* dest, T expected, T newValue);
 *     static T swap(volatile T* dest, T newValue);
 *     static T fetchAndAdd(volatile T* dest, T increment);
 * };
 *
 * All of the functions assume that the volatile T pointers are naturally aligned, and may not
 * operate as expected, if they are not so aligned.
 *
 * The behavior of the functions is analogous to the same-named member functions of the AtomicWord
 * template type in atomic_word.h.
 */

#pragma once

#include "mongo/config.h"

#if defined(MONGO_HAVE_CXX11_ATOMICS)
#error "Use of atomic_intrinsics.h is not supported when C++11 <atomic> is available"
#endif

#if defined(_WIN32)
#include "mongo/platform/atomic_intrinsics_win32.h"
#elif defined(MONGO_HAVE_GCC_ATOMIC_BUILTINS)
#include "mongo/platform/atomic_intrinsics_gcc_atomic.h"
#elif defined(MONGO_HAVE_GCC_SYNC_BUILTINS)
#include "mongo/platform/atomic_intrinsics_gcc_sync.h"
#elif defined(__i386__) || defined(__x86_64__)
#include "mongo/platform/atomic_intrinsics_gcc_intel.h"
#else
#error "Unsupported os/compiler family"
#endif
