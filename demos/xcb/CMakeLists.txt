find_library(LIBRARY_XCB xcb REQUIRED)

add_library(demo-xcb-base INTERFACE)
target_link_libraries(demo-xcb-base INTERFACE ${LIBRARY_XCB})

add_subdirectory(start)
add_subdirectory(cairo-start)
