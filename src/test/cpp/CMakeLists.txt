cmake_minimum_required(VERSION 3.9)

project(cpp_test)

add_executable(cpp_test "")
target_sources(cpp_test PUBLIC pch.cpp)

CONSUME_PAL(cpp_test)

if (MSVC)
    TARGET_CONFIG_MSVC_PCH(cpp_test pch.cpp pch.h)
    target_link_libraries(cpp_test windowsapp)
else()
    target_link_libraries(cpp_test c++ c++abi c++experimental)
    target_link_libraries(cpp_test -lpthread)
endif()

target_sources(cpp_test PUBLIC main.cpp)
