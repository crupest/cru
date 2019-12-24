find_package(GTest CONFIG REQUIRED)

include(GoogleTest)

add_subdirectory(win)

add_library(cru_test_base INTERFACE)
target_link_libraries(cru_test_base INTERFACE GTest::gtest GTest::gtest_main)
