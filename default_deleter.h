//
// Created by egor on 06.12.2020.
//

#ifndef PTRS_DEFAULT_DELETER_H
#define PTRS_DEFAULT_DELETER_H

namespace lab_05 {

template <typename T>
struct default_deleter {
    void operator()(T *pointer) {
        delete pointer;
    }
};

template <typename T>
struct default_deleter<T[]> {
    void operator()(T *pointer) {
        delete[] pointer;
    }
};
}  // namespace lab_05

#endif  // PTRS_DEFAULT_DELETER_H
