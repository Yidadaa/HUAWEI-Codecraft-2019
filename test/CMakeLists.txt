find_package(GTest REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS})

aux_source_directory(. DIR_TESTS)
add_executable(TestCodeCraft ${DIR_TESTS})

target_link_libraries(TestCodeCraft gtest gtest_main pthread Core)