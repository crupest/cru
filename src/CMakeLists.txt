add_subdirectory(ui)

if(WIN32)
add_subdirectory(win)
endif()

add_executable(demo main.cpp)

target_link_libraries(demo PRIVATE cru_ui)
