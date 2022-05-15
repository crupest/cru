include(Catch)

add_library(CruTestBase INTERFACE)
target_link_libraries(CruTestBase INTERFACE Catch2::Catch2WithMain)

add_subdirectory(common)
add_subdirectory(platform)
add_subdirectory(xml)
add_subdirectory(toml)
add_subdirectory(ui)
