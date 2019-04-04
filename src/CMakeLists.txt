add_library(cru_ui STATIC
    ui/content_control.cpp
    ui/control.cpp
    ui/layout_control.cpp
    ui/no_child_control.cpp
    ui/ui_manager.cpp
    ui/window.cpp
    ui/controls/button.cpp
    ui/controls/flex_layout.cpp
    ui/controls/text_block.cpp
    ui/render/border_render_object.cpp
    ui/render/flex_layout_render_object.cpp
    ui/render/render_object.cpp
    ui/render/text_render_object.cpp
    ui/render/window_render_object.cpp)

target_include_directories(cru_ui PUBLIC ${PROJECT_SOURCE_DIR}/include .)

if(WIN32)
add_subdirectory(platform_win)
target_link_libraries(cru_ui PUBLIC cru_platform_win)
endif()



add_executable(demo WIN32 main.cpp)

target_link_libraries(demo PRIVATE cru_ui)
