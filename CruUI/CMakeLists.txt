cmake_minimum_required(VERSION 3.11)
project(CruUI)

if (MSVC_VERSION GREATER_EQUAL "1900")
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("/std:c++latest" _cpp_latest_flag_supported)
    if (_cpp_latest_flag_supported)
        add_compile_options("/std:c++latest")
    endif()
endif()

include_directories(.)

add_executable(CruUI
        graph/graph.cpp
        ui/animations/animation.cpp
        ui/controls/button.cpp
        ui/controls/linear_layout.cpp
        ui/controls/margin_container.cpp
        ui/controls/text_block.cpp
        ui/controls/text_box.cpp
        ui/controls/toggle_button.cpp
        ui/events/ui_event.cpp
        ui/control.cpp
        ui/layout_base.cpp
        ui/ui_base.cpp
        ui/window.cpp
        application.cpp
        base.cpp
        exception.cpp
        main.cpp
        timer.cpp)
