project(ksio_tests LANGUAGES C)

option(PROJECT_BINARY_DIR "${CMAKE_BINARY_DIR}/tests")

add_executable(io_test io_test.c)
target_link_libraries(io_test ksio)

add_executable(midi_file_test midi_file_test.c)
target_link_libraries(midi_file_test ksio)
add_resources(midi_file_test resources)

add_executable(wav_file_test wav_file_test.c)
target_link_libraries(wav_file_test ksio)

add_executable(asperite_file_test asperite_file_test.c)
target_link_libraries(asperite_file_test ksio)
# add_resources(asperite_file_test resources)
