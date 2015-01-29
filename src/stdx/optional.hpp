// -*- C++ -*-
//===-------------------------- optional ----------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
// NOTE: THIS IS HAXOR MAGIC THAT JUST REMOVES THINGS FROM THE STANDARD
// NOTE: DO NOT USE THIS UNLESS YOU ARE MONGO
//
// TODO: work on actual license info
//
//===----------------------------------------------------------------------===//

#pragma once

#include <functional>
#include <stdexcept>
#include <initializer_list>
#include <type_traits>
#include <new>
#include <cassert>

namespace mongo {
namespace driver {

class bad_optional_access : public std::logic_error {
   public:
    explicit bad_optional_access(const std::string& __arg) : logic_error(__arg) {}
    explicit bad_optional_access(const char* __arg) : logic_error(__arg) {}
    bad_optional_access(const bad_optional_access&) = default;
    bad_optional_access& operator=(const bad_optional_access&) = default;

    // Get the key function ~bad_optional_access() into the dylib even if not
    // compiling for C++1y
    virtual ~bad_optional_access() {};
};

struct in_place_t {};
extern in_place_t in_place;

struct nullopt_t {
    explicit nullopt_t(int) {}
};

extern nullopt_t nullopt;

template <class _Tp, bool = std::is_trivially_destructible<_Tp>::value>
class __optional_storage {
   protected:
    typedef _Tp value_type;
    union {
        char __null_state_;
        value_type __val_;
    };
    bool __engaged_ = false;

    ~__optional_storage() {
        if (__engaged_) __val_.~value_type();
    }

    __optional_storage() : __null_state_('\0') {}

    __optional_storage(const __optional_storage& __x) : __engaged_(__x.__engaged_) {
        if (__engaged_) ::new (std::addressof(__val_)) value_type(__x.__val_);
    }

    __optional_storage(__optional_storage&& __x) : __engaged_(__x.__engaged_) {
        if (__engaged_) ::new (std::addressof(__val_)) value_type(std::move(__x.__val_));
    }

    __optional_storage(const value_type& __v) : __val_(__v), __engaged_(true) {}

    __optional_storage(value_type&& __v) : __val_(std::move(__v)), __engaged_(true) {}

    template <class... _Args>
    explicit __optional_storage(in_place_t, _Args&&... __args)
        : __val_(std::forward<_Args>(__args)...), __engaged_(true) {}
};

template <class _Tp>
class __optional_storage<_Tp, true> {
   protected:
    typedef _Tp value_type;
    union {
        char __null_state_;
        value_type __val_;
    };
    bool __engaged_ = false;

    __optional_storage() : __null_state_('\0') {}

    __optional_storage(const __optional_storage& __x) : __engaged_(__x.__engaged_) {
        if (__engaged_) ::new (std::addressof(__val_)) value_type(__x.__val_);
    }

    __optional_storage(__optional_storage&& __x) : __engaged_(__x.__engaged_) {
        if (__engaged_) ::new (std::addressof(__val_)) value_type(std::move(__x.__val_));
    }

    __optional_storage(const value_type& __v) : __val_(__v), __engaged_(true) {}

    __optional_storage(value_type&& __v) : __val_(std::move(__v)), __engaged_(true) {}

    template <class... _Args>
    explicit __optional_storage(in_place_t, _Args&&... __args)
        : __val_(std::forward<_Args>(__args)...), __engaged_(true) {}
};

template <class _Tp>
class optional : private __optional_storage<_Tp> {
    typedef __optional_storage<_Tp> __base;

   public:
    typedef _Tp value_type;

    static_assert(!std::is_reference<value_type>::value,
                  "Instantiation of optional with a reference type is ill-formed.");
    static_assert(!std::is_same<typename std::remove_cv<value_type>::type, in_place_t>::value,
                  "Instantiation of optional with a in_place_t type is ill-formed.");
    static_assert(!std::is_same<typename std::remove_cv<value_type>::type, nullopt_t>::value,
                  "Instantiation of optional with a nullopt_t type is ill-formed.");
    static_assert(std::is_object<value_type>::value,
                  "Instantiation of optional with a non-object type is "
                  "undefined behavior.");
    static_assert(std::is_nothrow_destructible<value_type>::value,
                  "Instantiation of optional with an object type that is not  "
                  "destructible is undefined behavior.");

    optional() {}
    optional(const optional&) = default;
    optional(optional&&) = default;
    ~optional() = default;
    optional(nullopt_t) {}
    optional(const value_type& __v) : __base(__v) {}
    optional(value_type&& __v) : __base(std::move(__v)) {}

    template <class... _Args, class = typename std::enable_if<
                                  std::is_constructible<value_type, _Args...>::value>::type>
    explicit optional(in_place_t, _Args&&... __args)
        : __base(in_place, std::forward<_Args>(__args)...) {}

    template <class _Up, class... _Args,
              class = typename std::enable_if<std::is_constructible<
                  value_type, std::initializer_list<_Up>&, _Args...>::value>::type>
    explicit optional(in_place_t, std::initializer_list<_Up> __il, _Args&&... __args)
        : __base(in_place, __il, std::forward<_Args>(__args)...) {}

    optional& operator=(nullopt_t) {
        if (this->__engaged_) {
            this->__val_.~value_type();
            this->__engaged_ = false;
        }
        return *this;
    }

    optional& operator=(const optional& __opt) {
        if (this->__engaged_ == __opt.__engaged_) {
            if (this->__engaged_) this->__val_ = __opt.__val_;
        } else {
            if (this->__engaged_)
                this->__val_.~value_type();
            else
                ::new (std::addressof(this->__val_)) value_type(__opt.__val_);
            this->__engaged_ = __opt.__engaged_;
        }
        return *this;
    }

    optional& operator=(optional&& __opt) {
        if (this->__engaged_ == __opt.__engaged_) {
            if (this->__engaged_) this->__val_ = std::move(__opt.__val_);
        } else {
            if (this->__engaged_)
                this->__val_.~value_type();
            else
                ::new (std::addressof(this->__val_)) value_type(std::move(__opt.__val_));
            this->__engaged_ = __opt.__engaged_;
        }
        return *this;
    }

    template <class _Up,
              class = typename std::enable_if<
                  std::is_same<typename std::remove_reference<_Up>::type, value_type>::value&&
                      std::is_constructible<value_type, _Up>::value&&
                          std::is_assignable<value_type&, _Up>::value>::type>
    optional& operator=(_Up&& __v) {
        if (this->__engaged_)
            this->__val_ = std::forward<_Up>(__v);
        else {
            ::new (std::addressof(this->__val_)) value_type(std::forward<_Up>(__v));
            this->__engaged_ = true;
        }
        return *this;
    }

    template <class... _Args, class = typename std::enable_if<
                                  std::is_constructible<value_type, _Args...>::value>::type>
    void emplace(_Args&&... __args) {
        *this = nullopt;
        ::new (std::addressof(this->__val_)) value_type(std::forward<_Args>(__args)...);
        this->__engaged_ = true;
    }

    template <class _Up, class... _Args,
              class = typename std::enable_if<std::is_constructible<
                  value_type, std::initializer_list<_Up>&, _Args...>::value>::type>
    void emplace(std::initializer_list<_Up> __il, _Args&&... __args) {
        *this = nullopt;
        ::new (std::addressof(this->__val_)) value_type(__il, std::forward<_Args>(__args)...);
        this->__engaged_ = true;
    }

    void swap(optional& __opt) {
        using std::swap;
        if (this->__engaged_ == __opt.__engaged_) {
            if (this->__engaged_) swap(this->__val_, __opt.__val_);
        } else {
            if (this->__engaged_) {
                ::new (std::addressof(__opt.__val_)) value_type(std::move(this->__val_));
                this->__val_.~value_type();
            } else {
                ::new (std::addressof(this->__val_)) value_type(std::move(__opt.__val_));
                __opt.__val_.~value_type();
            }
            swap(this->__engaged_, __opt.__engaged_);
        }
    }

    value_type const* operator->() const {
        assert(this->__engaged_);
        return std::addressof(this->__val_);
    }

    value_type* operator->() {
        assert(this->__engaged_);
        return std::addressof(this->__val_);
    }

    const value_type& operator*() const {
        assert(this->__engaged_);
        return this->__val_;
    }

    value_type& operator*() {
        assert(this->__engaged_);
        return this->__val_;
    }

    explicit operator bool() const { return this->__engaged_; }

    value_type& value() {
        if (!this->__engaged_) throw bad_optional_access("optional<T>::value: not engaged");
        return this->__val_;
    }

    const value_type& value() const {
        if (!this->__engaged_) throw bad_optional_access("optional<T>::value: not engaged");
        return this->__val_;
    }

    template <class _Up>
    value_type value_or(_Up&& __v) const& {
        static_assert(std::is_copy_constructible<value_type>::value,
                      "optional<T>::value_or: T must be copy constructible");
        static_assert(std::is_convertible<_Up, value_type>::value,
                      "optional<T>::value_or: U must be convertible to T");
        return this->__engaged_ ? this->__val_ : static_cast<value_type>(std::forward<_Up>(__v));
    }

    template <class _Up>
        value_type value_or(_Up&& __v) && {
        static_assert(std::is_move_constructible<value_type>::value,
                      "optional<T>::value_or: T must be move constructible");
        static_assert(std::is_convertible<_Up, value_type>::value,
                      "optional<T>::value_or: U must be convertible to T");
        return this->__engaged_ ? std::move(this->__val_)
                                : static_cast<value_type>(std::forward<_Up>(__v));
    }

   private:
    value_type const* __operator_arrow(std::true_type) const {
        return std::addressof(this->__val_);
    }

    value_type const* __operator_arrow(std::false_type) const { return &this->__val_; }
};

template <class _Tp>
inline bool operator==(const optional<_Tp>& __x, nullopt_t) {
    return !static_cast<bool>(__x);
}

template <class _Tp>
inline bool operator==(nullopt_t, const optional<_Tp>& __x) {
    return !static_cast<bool>(__x);
}

template <class _Tp>
inline bool operator<(const optional<_Tp>&, nullopt_t) {
    return false;
}

template <class _Tp>
inline bool operator<(nullopt_t, const optional<_Tp>& __x) {
    return static_cast<bool>(__x);
}

template <class _Tp>
inline bool operator==(const optional<_Tp>& __x, const _Tp& __v) {
    return static_cast<bool>(__x) ? *__x == __v : false;
}

template <class _Tp>
inline bool operator==(const _Tp& __v, const optional<_Tp>& __x) {
    return static_cast<bool>(__x) ? *__x == __v : false;
}
}
}
