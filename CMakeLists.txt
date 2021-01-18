cmake_minimum_required(VERSION 3.5)

project(ksio LANGUAGES C)

function(add_resources target dir)
    file(GLOB_RECURSE  ${target}_resources RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${dir}/*)

    foreach(var ${${target}_resources})
        add_custom_command(TARGET ${target} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/${var} $<TARGET_FILE_DIR:${target}>/${var})
    endforeach()
endfunction()

file(GLOB_RECURSE ksio_src ksio/*.c ksio/*.h)
add_library(ksio ${ksio_src})
target_include_directories(ksio PUBLIC .)

set_property(TARGET ksio PROPERTY C_STANDARD 11)

add_subdirectory(tests)


