//
// Created by egor on 04.12.2020.
//
#include <memory>
#include <type_traits>
#include "default_deleter.h"

#ifndef PTRS_UNIQUE_PTR_H
#define PTRS_UNIQUE_PTR_H

namespace lab_05::unique {

template <typename T, typename Deleter = lab_05::default_deleter<T>>
struct unique_ptr {
private:
    using ptr_type = typename std::remove_extent<T>::type;

    ptr_type *data;

    Deleter deleter;

public:
    explicit unique_ptr(ptr_type *ptr = nullptr,
                        const Deleter &_deleter = Deleter())
        : data(ptr), deleter(_deleter) {
    }

    // cppcheck-suppress copyCtorPointerCopying
    unique_ptr(ptr_type *ptr, Deleter &&_deleter)
        : data(ptr), deleter(std::move(_deleter)) {
    }

    // cppcheck-suppress copyCtorPointerCopying
    unique_ptr(unique_ptr &&other)
        : data(std::exchange(other.data, nullptr)),
          deleter(std::move(other.deleter)) {
    }

    unique_ptr(const unique_ptr &) = delete;

    ~unique_ptr() {
        reset();
    }

    unique_ptr &operator=(const unique_ptr &) = delete;

    unique_ptr &operator=(unique_ptr &&other) {
        if (this == &other) {
            return *this;
        }

        deleter(data);

        data = std::exchange(other.data, nullptr);
        deleter = other.deleter;

        return *this;
    }

    ptr_type &operator[](std::size_t index) const {
        return *(data + index);
    }

    ptr_type &operator*() const {
        return *data;
    }

    ptr_type *operator->() const {
        return data;
    }

    bool operator!() const {
        return data == nullptr;
    }

    bool operator==(const unique_ptr &other) const {
        return data == other.data;
    }

    bool operator!=(const unique_ptr &other) const {
        return data != other.data;
    }

    void reset(ptr_type *ptr = nullptr) {
        deleter(data);
        data = ptr;
    }

    ptr_type *get() const {
        return data;
    }

    ptr_type *release() {
        return std::exchange(data, nullptr);
    }
};
}  // namespace lab_05::unique

template <typename T>
void swap(lab_05::unique::unique_ptr<T> &p1,
          lab_05::unique::unique_ptr<T> &p2) {
    lab_05::unique::unique_ptr<T> tmp = std::move(p1);
    p1 = std::move(p2);
    p2 = std::move(tmp);
}

#endif  // PTRS_UNIQUE_PTR_H
