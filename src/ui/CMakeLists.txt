set(CRU_UI_INCLUDE_DIR ${CRU_INCLUDE_DIR}/cru/ui)

add_library(cru_ui STATIC
	helper.hpp
	routed_event_dispatch.hpp

	click_detector.cpp
    content_control.cpp
	control.cpp
	helper.cpp
    layout_control.cpp
    no_child_control.cpp
    ui_manager.cpp
    window.cpp
    controls/button.cpp
    controls/container.cpp
    controls/flex_layout.cpp
    controls/stack_layout.cpp
    controls/text_block.cpp
    controls/text_box.cpp
	controls/text_control_service.hpp
    render/border_render_object.cpp
    render/canvas_render_object.cpp
    render/flex_layout_render_object.cpp
    render/layout_utility.cpp
    render/render_object.cpp
    render/stack_layout_render_object.cpp
    render/text_render_object.cpp
    render/window_render_object.cpp
)
target_sources(cru_ui PUBLIC
	${CRU_UI_INCLUDE_DIR}/base.hpp
	${CRU_UI_INCLUDE_DIR}/click_detector.hpp
	${CRU_UI_INCLUDE_DIR}/content_control.hpp
	${CRU_UI_INCLUDE_DIR}/control.hpp
	${CRU_UI_INCLUDE_DIR}/layout_control.hpp
	${CRU_UI_INCLUDE_DIR}/no_child_control.hpp
	${CRU_UI_INCLUDE_DIR}/ui_event.hpp
	${CRU_UI_INCLUDE_DIR}/ui_manager.hpp
	${CRU_UI_INCLUDE_DIR}/window.hpp
	${CRU_UI_INCLUDE_DIR}/controls/button.hpp
	${CRU_UI_INCLUDE_DIR}/controls/container.hpp
	${CRU_UI_INCLUDE_DIR}/controls/flex_layout.hpp
	${CRU_UI_INCLUDE_DIR}/controls/stack_layout.hpp
	${CRU_UI_INCLUDE_DIR}/controls/text_box.hpp
	${CRU_UI_INCLUDE_DIR}/controls/text_block.hpp
	${CRU_UI_INCLUDE_DIR}/render/base.hpp
	${CRU_UI_INCLUDE_DIR}/render/border_render_object.hpp
	${CRU_UI_INCLUDE_DIR}/render/canvas_render_object.hpp
	${CRU_UI_INCLUDE_DIR}/render/flex_layout_render_object.hpp
	${CRU_UI_INCLUDE_DIR}/render/layout_render_object.hpp
	${CRU_UI_INCLUDE_DIR}/render/layout_utility.hpp
	${CRU_UI_INCLUDE_DIR}/render/render_object.hpp
    ${CRU_UI_INCLUDE_DIR}/render/stack_layout_render_object.hpp
	${CRU_UI_INCLUDE_DIR}/render/text_render_object.hpp
	${CRU_UI_INCLUDE_DIR}/render/window_render_object.hpp
)
target_link_libraries(cru_ui PUBLIC cru_platform_native)
