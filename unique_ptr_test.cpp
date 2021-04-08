#ifdef TEST_STD_PTRS
#include <memory>
#else

#include "unique_ptr.h"

#endif

#include <iostream>
#include <type_traits>
#include <utility>
#include "doctest.h"

namespace {
#ifdef TEST_STD_PTRS
using std::unique_ptr;
#else
using lab_05::unique::unique_ptr;
#endif

TEST_CASE(
#ifdef TEST_STD_PTRS
    "std::unique_ptr<int> "
#else
    "lab_05::unique::unique_ptr<int> "
#endif
    "default constructor") {
    const unique_ptr<int> p;
    CHECK(p.get() == nullptr);
    CHECK(!p);
}

TEST_CASE("unique_ptr nullptr") {
    const unique_ptr<int> p(nullptr);
    CHECK(p.get() == nullptr);
    CHECK(!p);
}

TEST_CASE("unique_ptr new-delete") {
    const unique_ptr<int> p(new int(10));
    CHECK(p);
}

TEST_CASE("unique_ptr constructor is explicit") {
    CHECK(!std::is_convertible_v<int *, unique_ptr<int>>);
}

//
TEST_CASE("unique_ptr new[]-delete[]") {
    const unique_ptr<int[]> p(new int[5]{});
}

TEST_CASE("unique_ptr custom copyable deleter") {
    struct Deleter {
        int *counter;  // NOLINT(misc-non-private-member-variables-in-classes)
        // NOLINTNEXTLINE(readability-make-member-function-const)
        void operator()(int *) {
            ++*counter;
        }
    };
    int data = 10;
    int counter = 0;
    const Deleter d{&counter};
    CHECK(counter == 0);
    {
        const unique_ptr<int, Deleter> p(&data, d);
        CHECK(counter == 0);
    }
    CHECK(counter == 1);
    {
        const unique_ptr<int[], Deleter> p(&data, d);
        CHECK(counter == 1);
    }
    CHECK(counter == 2);
}

TEST_CASE("unique_ptr custom move-only deleter") {
    struct Deleter {
        int *counter;  // NOLINT(misc-non-private-member-variables-in-classes)
        explicit Deleter(int *counter_) : counter(counter_) {
        }

        Deleter(const Deleter &) = delete;

        Deleter &operator=(const Deleter &) = delete;

        Deleter(Deleter &&) = default;

        Deleter &operator=(Deleter &&) = default;

        ~Deleter() = default;

        // NOLINTNEXTLINE(readability-make-member-function-const)
        void operator()(int *) {
            ++*counter;
        }
    };
    int data = 10;
    int counter = 0;
    CHECK(counter == 0);
    {
        Deleter d{&counter};
        const unique_ptr<int, Deleter> p(&data, std::move(d));
        CHECK(counter == 0);
    }
    CHECK(counter == 1);
    {
        Deleter d{&counter};
        const unique_ptr<int[], Deleter> p(&data, std::move(d));
        CHECK(counter == 1);
    }
    CHECK(counter == 2);
}

TEST_CASE("unique_ptr non-copyable") {
    CHECK(!std::is_copy_constructible_v<unique_ptr<int>>);
    CHECK(!std::is_copy_assignable_v<unique_ptr<int>>);
    CHECK(!std::is_copy_constructible_v<unique_ptr<int[]>>);
    CHECK(!std::is_copy_assignable_v<unique_ptr<int[]>>);
}

TEST_CASE("unique_ptr non-array getters") {
    struct Foo {
        int value = 10;
    };

    CHECK(!std::is_convertible_v<unique_ptr<Foo>, bool>);
    SUBCASE("empty") {
        const unique_ptr<Foo> p;
        CHECK(p.get() == nullptr);
        CHECK(!p);
    }
    SUBCASE("non-empty non-array") {
        Foo *raw_p = new Foo();
        const unique_ptr<Foo> p(raw_p);
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
}

TEST_CASE("unique_ptr array getters") {
    CHECK(!std::is_convertible_v<unique_ptr<int[]>, bool>);
    SUBCASE("empty") {
        const unique_ptr<int[]> p;
        CHECK(p.get() == nullptr);
        CHECK(!p);
    }
    SUBCASE("non-empty non-array") {
        int *raw_p = new int[4]{50, 60, 70, 80};
        const unique_ptr<int[]> p(raw_p);
        REQUIRE(p.get() == raw_p);
        REQUIRE(p);

        CHECK(p[0] == 50);
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        CHECK(&p[0] == &raw_p[0]);

        CHECK(p[3] == 80);
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        CHECK(&p[3] == &raw_p[3]);

        p[0] = 55;
        CHECK(p[0] == 55);
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        CHECK(&p[0] == &raw_p[0]);
    }
}

//
//// TODO: check non-default-constructible deleter.
//// TODO: check that deleters are moved as well (not copied).

TEST_CASE("unique_ptr move constructor") {
    int *raw_p = new int(10);
    unique_ptr<int> p1(raw_p);
    const unique_ptr<int> p2(std::move(p1));

    CHECK(p1.get() == nullptr);  // NOLINT(bugprone-use-after-move)
    CHECK(!p1);

    REQUIRE(p2.get() == raw_p);
    REQUIRE(p2);
    CHECK(&*p2 == raw_p);
}

TEST_CASE("unique_ptr move assignment over empty") {
    int *raw_p = new int(10);
    unique_ptr<int> p1(raw_p);
    unique_ptr<int> p2;

    p2 = std::move(p1);

    CHECK(p1.get() == nullptr);  // NOLINT(bugprone-use-after-move)
    CHECK(!p1);

    REQUIRE(p2.get() == raw_p);
    REQUIRE(p2);
    CHECK(&*p2 == raw_p);
}

TEST_CASE("unique_ptr move assignment over non-empty") {
    int *raw_p = new int(10);
    unique_ptr<int> p1(raw_p);
    unique_ptr<int> p2(new int(20));

    p2 = std::move(p1);

    CHECK(p1.get() == nullptr);  // NOLINT(bugprone-use-after-move)
    CHECK(!p1);

    REQUIRE(p2.get() == raw_p);
    REQUIRE(p2);
    CHECK(&*p2 == raw_p);
}

//
// See
// https://stackoverflow.com/questions/13127455/what-does-the-standard-library-guarantee-about-self-move-assignment
// and
// https://stackoverflow.com/questions/13129031/on-implementing-stdswap-in-terms-of-move-assignment-and-move-constructor
// I believe that requiring move self-assignment to be a no-op is a safer bet.
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-move"
#endif
TEST_CASE("unique_ptr move self-assignment of empty") {
    unique_ptr<int> p;
    p = std::move(p);
    CHECK(p.get() == nullptr);  // NOLINT(bugprone-use-after-move)
    CHECK(!p);
}

TEST_CASE("unique_ptr move self-assignment of non-empty") {
    int *raw_p = new int(10);
    unique_ptr<int> p(raw_p);
    p = std::move(p);
    REQUIRE(p.get() == raw_p);  // NOLINT(bugprone-use-after-move)
    REQUIRE(p);
    CHECK(&*p == raw_p);
}

#ifdef __clang__
#pragma clang diagnostic pop
#endif

TEST_CASE("unique_ptr reset from empty") {
    unique_ptr<int> p;
    int *raw_p = nullptr;

    SUBCASE("no arguments") {
        p.reset();
    }
    SUBCASE("to nullptr") {
        p.reset(raw_p);
    }
    SUBCASE("to a new pointer") {
        raw_p = new int(20);
        p.reset(raw_p);
    }

    REQUIRE(p.get() == raw_p);
    if (raw_p != nullptr) {
        REQUIRE(p);
        CHECK(&*p == raw_p);
    } else {
        REQUIRE(!p);
    }
}

TEST_CASE("unique_ptr reset from non-empty") {
    unique_ptr<int> p(new int(10));
    int *raw_p = nullptr;

    SUBCASE("no arguments") {
        p.reset();
    }
    SUBCASE("to nullptr") {
        p.reset(raw_p);
    }
    SUBCASE("to a new pointer") {
        raw_p = new int(20);
        p.reset(raw_p);
    }

    REQUIRE(p.get() == raw_p);
    if (raw_p != nullptr) {
        REQUIRE(p);
        CHECK(&*p == raw_p);
    } else {
        REQUIRE(!p);
    }
}

TEST_CASE("unique_ptr release from empty") {
    unique_ptr<int> p;
    CHECK(p.release() == nullptr);
    CHECK(!p);
    CHECK(p.get() == nullptr);
}

TEST_CASE("unique_ptr release from non-empty") {
    int *raw_p = new int(10);
    unique_ptr<int> p(raw_p);
    CHECK(p.release() == raw_p);
    delete raw_p;

    CHECK(!p);
    CHECK(p.get() == nullptr);
}

//
// TODO: check that deleters are swapped as well.
TEST_CASE("unique_ptr swap") {
    int *raw_p1 = new int(10);
    int *raw_p2 = new int(20);
    unique_ptr<int> p1(raw_p1);
    unique_ptr<int> p2(raw_p2);
    swap(p1, p2);

    CHECK(p1);
    CHECK(p2);
    CHECK(p1.get() == raw_p2);
    CHECK(p2.get() == raw_p1);
}

TEST_CASE("unique_ptr operator== and operator!=") {
    const unique_ptr<int> null1;
    const unique_ptr<int> null2;
    const unique_ptr<int> p1(new int(10));
    const unique_ptr<int> p2(new int(10));

    CHECK(null1 == null1);

    CHECK(null1 == null2);
    CHECK(null1 != p1);
    CHECK(null1 != p2);

    CHECK(null2 == null1);
    CHECK(p1 != null1);
    CHECK(p2 != null1);

    CHECK(p1 == p1);
    CHECK(p2 == p2);
    CHECK(p1 != p2);
    CHECK(p2 != p1);
}
/*
 */

// TODO: test with incomplete types
// TODO: test implicit constructors

}  // namespace
