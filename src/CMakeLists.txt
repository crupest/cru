add_library(cru_ui STATIC
    application.cpp
    cru_debug.cpp
    exception.cpp
    timer.cpp
    graph/graph_manager.cpp
    graph/window_render_target.cpp
    ui/content_control.cpp
    ui/control.cpp
    ui/input_util.cpp
    ui/layout_control.cpp
    ui/no_child_control.cpp
    ui/ui_manager.cpp
    ui/window_class.cpp
    ui/window.cpp
    ui/animations/animation.cpp
    ui/controls/button.cpp
    ui/controls/flex_layout.cpp
    ui/controls/text_block.cpp
    ui/render/border_render_object.cpp
    ui/render/flex_layout_render_object.cpp
    ui/render/render_object.cpp
    ui/render/text_render_object.cpp
    ui/render/window_render_object.cpp
    util/string_util.cpp)


target_include_directories(cru_ui PUBLIC .)

if(WIN32)
find_library(D3D11_LIB D3D11)
find_library(D2D1_LIB D2d1)
find_library(DWRITE_LIB DWrite)

target_link_libraries(cru_ui PRIVATE ${D3D11_LIB} ${D2D1_LIB} ${DWRITE_LIB})

target_compile_definitions(cru_ui PUBLIC UNICODE _UNICODE)
endif()

add_executable(demo main.cpp)
target_link_libraries(demo PRIVATE cru_ui)
