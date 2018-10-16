cmake_minimum_required(VERSION 3.9)

project(cpp_test)

add_executable(cpp_test "")
target_sources(cpp_test PUBLIC pch.cpp)
target_include_directories(cpp_test BEFORE PUBLIC "${PROJECT_SOURCE_DIR}/Generated Files")

if (MSVC)
    TARGET_CONFIG_MSVC_PCH(cpp_test pch.cpp pch.h)
    target_link_libraries(cpp_test windowsapp)
else()
    target_link_libraries(cpp_test c++ c++abi c++experimental)
    target_link_libraries(cpp_test -lpthread)
endif()

target_sources(cpp_test PUBLIC main.cpp)

add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD COMMAND ${PROJECT_SOURCE_DIR}/prebuild.bat)
