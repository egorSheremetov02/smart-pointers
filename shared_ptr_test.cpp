#ifdef TEST_STD_PTRS
#include <memory>
#else

#include "shared_ptr.h"
#include "shared_ptr.h"  // Ensure include guards are present.

#endif

#include <iostream>  // Требуется из-за бага в библиотеке doctest: https://github.com/onqtam/doctest/issues/356
#include <type_traits>
#include <utility>
#include "doctest.h"

namespace {

#ifdef TEST_STD_PTRS
using std::shared_ptr;
#else
using lab_05::shared::shared_ptr;
#endif

TEST_CASE(
#ifdef TEST_STD_PTRS
    "std::shared_ptr<int> "
#else
    "lab_05::shared::shared_ptr<int> "
#endif
    "default constructor") {
    const shared_ptr<int> p;
    CHECK(p.get() == nullptr);
    CHECK(!p);
}

TEST_CASE("shared_ptr nullptr") {
    const shared_ptr<int> p(nullptr);
    CHECK(p.get() == nullptr);
    CHECK(!p);
}

TEST_CASE("shared_ptr new-delete") {
    const shared_ptr<int> p(new int(10));
    CHECK(p);
}

TEST_CASE("shared_ptr constructor is explicit") {
    CHECK(!std::is_convertible_v<int *, shared_ptr<int>>);
}

TEST_CASE("shared_ptr non-empty getters") {
    struct Foo {
        int value = 10;
    };

    CHECK(!std::is_convertible_v<shared_ptr<Foo>, bool>);
    Foo *raw_p = new Foo();
    const shared_ptr<Foo> p(raw_p);
    // REQUIRE() stops the test in case of failure.
    REQUIRE(p.get() == raw_p);
    REQUIRE(p);

    CHECK((*p).value == 10);
    CHECK(&*p == raw_p);

    (*p).value = 20;
    CHECK((*p).value == 20);
    CHECK(&*p == raw_p);

    CHECK(p->value == 20);
}

void check_after_copy(int *raw_p,
                      const shared_ptr<int> &p1,
                      const shared_ptr<int> &p2) {
    REQUIRE(p2.get() == raw_p);
    REQUIRE(p2);
    CHECK(&*p2 == raw_p);

    REQUIRE(p1.get() == raw_p);
    REQUIRE(p1);
    CHECK(&*p1 == raw_p);
}

TEST_CASE("shared_ptr copy constructor") {
    int *raw_p = new int(10);
    const shared_ptr<int> p1(raw_p);
    // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
    const shared_ptr<int> p2(p1);
    check_after_copy(raw_p, p1, p2);
}

TEST_CASE("shared_ptr copy assignment over empty") {
    int *raw_p = new int(10);
    const shared_ptr<int> p1(raw_p);
    shared_ptr<int> p2;
    p2 = p1;
    check_after_copy(raw_p, p1, p2);
}

TEST_CASE("shared_ptr copy assignment over non-empty") {
    int *raw_p = new int(10);
    const shared_ptr<int> p1(raw_p);
    shared_ptr<int> p2(new int(20));
    p2 = p1;
    check_after_copy(raw_p, p1, p2);
}

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-assign-overloaded"
#endif
TEST_CASE("shared_ptr copy self-assignment of empty") {
    shared_ptr<int> p;
    p = p;
    CHECK(p.get() == nullptr);
    CHECK(!p);
}

TEST_CASE("shared_ptr copy self-assignment of non-empty") {
    int *raw_p = new int(10);
    shared_ptr<int> p(raw_p);
    p = p;
    REQUIRE(p.get() == raw_p);
    REQUIRE(p);
    CHECK(&*p == raw_p);
}

#ifdef __clang__
#pragma clang diagnostic pop
#endif

void check_after_move(int *raw_p,
                      const shared_ptr<int> &p1,
                      const shared_ptr<int> &p2) {
    REQUIRE(p2.get() == raw_p);
    REQUIRE(p2);
    CHECK(&*p2 == raw_p);

    CHECK(p1.get() == nullptr);
    CHECK(!p1);
}

TEST_CASE("shared_ptr move constructor") {
    int *raw_p = new int(10);
    shared_ptr<int> p1(raw_p);
    const shared_ptr<int> p2(std::move(p1));
    // cppcheck-suppress accessMoved
    // NOLINTNEXTLINE(bugprone-use-after-move)
    check_after_move(raw_p, p1, p2);
}

TEST_CASE("shared_ptr move assignment over empty") {
    int *raw_p = new int(10);
    shared_ptr<int> p1(raw_p);
    shared_ptr<int> p2;
    p2 = std::move(p1);
    // cppcheck-suppress accessMoved
    // NOLINTNEXTLINE(bugprone-use-after-move)
    check_after_move(raw_p, p1, p2);
}

TEST_CASE("shared_ptr move assignment over non-empty") {
    int *raw_p = new int(10);
    shared_ptr<int> p1(raw_p);
    shared_ptr<int> p2(new int(20));
    p2 = std::move(p1);
    // cppcheck-suppress accessMoved
    // NOLINTNEXTLINE(bugprone-use-after-move)
    check_after_move(raw_p, p1, p2);
}

// See
// https://stackoverflow.com/questions/13127455/what-does-the-standard-library-guarantee-about-self-move-assignment
// and
// https://stackoverflow.com/questions/13129031/on-implementing-stdswap-in-terms-of-move-assignment-and-move-constructor
// I believe that requiring move self-assignment to be a no-op is a safer bet.
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-move"
#endif
TEST_CASE("shared_ptr move self-assignment of empty") {
    shared_ptr<int> p;
    p = std::move(p);
    CHECK(p.get() == nullptr);  // NOLINT(bugprone-use-after-move)
    CHECK(!p);
}

TEST_CASE("shared_ptr move self-assignment of non-empty") {
    int *raw_p = new int(10);
    shared_ptr<int> p(raw_p);
    p = std::move(p);
    REQUIRE(p.get() == raw_p);  // NOLINT(bugprone-use-after-move)
    REQUIRE(p);
    CHECK(&*p == raw_p);
}

#ifdef __clang__
#pragma clang diagnostic pop
#endif

TEST_CASE("shared_ptr swap") {
    int *raw_p1 = new int(10);
    int *raw_p2 = new int(20);
    shared_ptr<int> p1(raw_p1);
    shared_ptr<int> p2(raw_p2);
    swap(p1, p2);

    CHECK(p1);
    CHECK(p2);
    CHECK(p1.get() == raw_p2);
    CHECK(p2.get() == raw_p1);
}

TEST_CASE("shared_ptr operator== and operator!=") {
    const shared_ptr<int> null1;
    const shared_ptr<int> null2;
    const shared_ptr<int> p1a(new int(10));
    // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
    const shared_ptr<int> p1b(p1a);
    const shared_ptr<int> p2(new int(10));

    CHECK(null1 == null1);

    CHECK(null1 == null2);
    CHECK(null1 != p1a);
    CHECK(null1 != p1b);
    CHECK(null1 != p2);

    CHECK(null2 == null1);
    CHECK(p1a != null1);
    CHECK(p1b != null1);
    CHECK(p2 != null1);

    CHECK(p1a == p1a);
    CHECK(p1a == p1b);
    CHECK(p1a != p2);

    CHECK(p1b == p1a);
    CHECK(p1b == p1b);
    CHECK(p1b != p2);

    CHECK(p2 != p1a);
    CHECK(p2 != p1b);
    CHECK(p2 == p2);
}
/*
 */

// TODO: test with incomplete types
// TODO: test reset()
// TODO: test implicit constructors

}  // namespace
