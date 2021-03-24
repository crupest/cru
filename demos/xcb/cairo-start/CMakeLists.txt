find_library(LIBRARY_CAIRO cairo REQUIRED)

add_executable(demo-xcb-cairo-start main.cpp)
target_link_libraries(demo-xcb-cairo-start PUBLIC demo-xcb-base ${LIBRARY_CAIRO})
