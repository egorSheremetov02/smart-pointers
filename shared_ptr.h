//
// Created by egor on 04.12.2020.
//

#include <memory>
#include <type_traits>
#include "default_deleter.h"

#ifndef PTRS_SHARED_PTR_H
#define PTRS_SHARED_PTR_H

namespace lab_05::shared {

template <typename T>
struct shared_ptr {
private:
    using ptr_type = typename std::remove_extent<T>::type;

    struct storage {
        ptr_type *ptr;
        int counter;
    };

    storage *my_storage;

public:
    explicit shared_ptr(ptr_type *ptr = nullptr) {
        if (ptr == nullptr) {
            my_storage = nullptr;
        } else {
            my_storage = new storage{ptr, 1};
        }
    }

    // cppcheck-suppress copyCtorPointerCopying
    shared_ptr(const shared_ptr &other) : my_storage(other.my_storage) {
        my_storage->counter++;
    }

    // cppcheck-suppress copyCtorPointerCopying
    shared_ptr(shared_ptr &&other)
        : my_storage(std::exchange(other.my_storage, nullptr)) {
    }

    shared_ptr &operator=(const shared_ptr &other) {
        if (this == &other) {
            return *this;
        }

        reset();

        my_storage = other.my_storage;
        my_storage->counter++;
        return *this;
    }

    shared_ptr &operator=(shared_ptr &&other) {
        if (this == &other) {
            return *this;
        }

        reset();

        my_storage = std::exchange(other.my_storage, nullptr);
        return *this;
    }

    ~shared_ptr() {
        reset();
    }

    bool operator!() const {
        return my_storage == nullptr;
    }

    ptr_type &operator[](std::size_t index) const {
        return *(get() + index);
    }

    ptr_type &operator*() const {
        return *get();
    }

    ptr_type *operator->() const {
        return get();
    }

    bool operator==(const shared_ptr &other) const {
        return other.my_storage == this->my_storage;
    }

    bool operator!=(const shared_ptr &other) const {
        return other.my_storage != this->my_storage;
    }

    ptr_type *get() const {
        if (!my_storage) {
            return nullptr;
        }
        return my_storage->ptr;
    }

    void reset() {
        if (!my_storage) {
            return;
        }
        my_storage->counter--;
        if (my_storage->counter == 0) {
            lab_05::default_deleter<T>()(my_storage->ptr);

            // valid only in case we are sure that no one else is using this
            // in our case it must be true
            delete my_storage;
        }
    }
};

}  // namespace lab_05::shared

template <typename T>
void swap(lab_05::shared::shared_ptr<T> &p1,
          lab_05::shared::shared_ptr<T> &p2) {
    // we do not use move in this case since objects are basic and copying is
    // cheap
    lab_05::shared::shared_ptr<T> tmp = p1;
    p1 = p2;
    p2 = tmp;
}

#endif  // PTRS_SHARED_PTR_H
