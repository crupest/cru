add_library(cru_ui STATIC
    content_control.cpp
    control.cpp
    layout_control.cpp
    no_child_control.cpp
    ui_manager.cpp
    window.cpp
    controls/button.cpp
    controls/flex_layout.cpp
    controls/text_block.cpp
    render/border_render_object.cpp
    render/flex_layout_render_object.cpp
    render/render_object.cpp
    render/text_render_object.cpp
    render/window_render_object.cpp)

target_include_directories(cru_ui PUBLIC ${PROJECT_SOURCE_DIR}/include .)

if(WIN32)
target_link_libraries(cru_ui PUBLIC cru_win_native)
endif()

