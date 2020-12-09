project(ksio_tests LANGUAGES C)

option(PROJECT_BINARY_DIR "${CMAKE_BINARY_DIR}/tests")

add_executable(io_test io_test.c)
target_link_libraries(io_test ksio)
