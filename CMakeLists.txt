cmake_minimum_required(VERSION 3.10)

project(ptrs)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if (MSVC)
    add_compile_options(/W4 /WX)
    # Do not specify /O2 because there are Debug/Release modes.
else()
    if (NOT DEFINED CMAKE_BUILD_TYPE)
      set(CMAKE_BUILD_TYPE Debug)  # Add debug information
    endif (NOT DEFINED CMAKE_BUILD_TYPE)

    add_compile_options(-Wall -Wextra -Werror -O2)
    if (UNIX AND NOT DEFINED CMAKE_CXX_FLAGS)  # Do not add if -DCMAKE_CXX_FLAGS is passed
      add_compile_options(-fsanitize=address)
      add_link_options(-fsanitize=address)
    endif (UNIX AND NOT DEFINED CMAKE_CXX_FLAGS)
endif (MSVC)

add_executable(ptrs-test main.cpp unique_ptr_test.cpp shared_ptr_test.cpp)

add_executable(ptrs-test-std main.cpp unique_ptr_test.cpp shared_ptr_test.cpp)
target_compile_definitions(ptrs-test-std PUBLIC -DTEST_STD_PTRS)
