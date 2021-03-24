find_package(GTest CONFIG REQUIRED)

include(GoogleTest)

add_library(cru_test_base INTERFACE)
target_link_libraries(cru_test_base INTERFACE GTest::gtest GTest::gtest_main)

add_subdirectory(common)
add_subdirectory(platform)

if(WIN32)
	add_subdirectory(win)
endif()
