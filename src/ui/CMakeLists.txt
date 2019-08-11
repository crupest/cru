file(GLOB CRU_UI_INCLUDE_DIR ${CRU_INCLUDE_DIR}/cru/ui)

add_library(cru_ui STATIC
	routed_event_dispatch.hpp

	click_detector.cpp
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
    render/window_render_object.cpp
)
target_sources(cru_ui PUBLIC
	${CRU_UI_INCLUDE_DIR}/base.hpp
	${CRU_UI_INCLUDE_DIR}/click_detector.hpp
	${CRU_UI_INCLUDE_DIR}/content_control.hpp
	${CRU_UI_INCLUDE_DIR}/control.hpp
	${CRU_UI_INCLUDE_DIR}/layout_control.hpp
	${CRU_UI_INCLUDE_DIR}/no_child_control.hpp
	${CRU_UI_INCLUDE_DIR}/ui_manager.hpp
	${CRU_UI_INCLUDE_DIR}/window.hpp
	${CRU_UI_INCLUDE_DIR}/controls/button.hpp
	${CRU_UI_INCLUDE_DIR}/controls/flex_layout.hpp
	${CRU_UI_INCLUDE_DIR}/controls/text_block.hpp
	${CRU_UI_INCLUDE_DIR}/event/ui_event.hpp
	${CRU_UI_INCLUDE_DIR}/render/border_render_object.hpp
	${CRU_UI_INCLUDE_DIR}/render/flex_layout_render_object.hpp
	${CRU_UI_INCLUDE_DIR}/render/render_object.hpp
	${CRU_UI_INCLUDE_DIR}/render/text_render_object.hpp
	${CRU_UI_INCLUDE_DIR}/render/window_render_object.hpp
)
target_link_libraries(cru_ui PUBLIC cru_base)

if(WIN32)
target_link_libraries(cru_ui PUBLIC cru_win_native)
endif()
