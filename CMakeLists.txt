cmake_minimum_required(VERSION 3.5)

project(ksio LANGUAGES C)

file(GLOB_RECURSE ksio_src ksio/*.c ksio/*.h)
add_library(ksio ${ksio_src})
target_include_directories(ksio PUBLIC .)
set_property(TARGET ksio PROPERTY C_STANDARD 11)

add_subdirectory(tests)


