set(CRU_UI_INCLUDE_DIR ${CRU_INCLUDE_DIR}/cru/ui)

add_library(cru_ui STATIC
	Helper.hpp
	host/RoutedEventDispatch.hpp

	Helper.cpp
	UiManager.cpp
	controls/Button.cpp
	controls/Container.cpp
	controls/ContentControl.cpp
	controls/Control.cpp
	controls/FlexLayout.cpp
	controls/LayoutControl.cpp
	controls/NoChildControl.cpp
	controls/StackLayout.cpp
	controls/TextBlock.cpp
	controls/TextBox.cpp
	controls/TextControlService.hpp
	controls/Window.cpp
	events/UiEvent.cpp
	helper/ClickDetector.cpp
	helper/ShortcutHub.cpp
	host/LayoutPaintCycler.cpp
	host/WindowHost.cpp
	render/BorderRenderObject.cpp
	render/CanvasRenderObject.cpp
	render/FlexLayoutRenderObject.cpp
	render/LayoutHelper.cpp
	render/RenderObject.cpp
	render/ScrollRenderObject.cpp
	render/StackLayoutRenderObject.cpp
	render/TextRenderObject.cpp
)
target_sources(cru_ui PUBLIC
	${CRU_UI_INCLUDE_DIR}/Base.hpp
	${CRU_UI_INCLUDE_DIR}/DebugFlags.hpp
	${CRU_UI_INCLUDE_DIR}/UiManager.hpp
	${CRU_UI_INCLUDE_DIR}/controls/Base.hpp
	${CRU_UI_INCLUDE_DIR}/controls/Button.hpp
	${CRU_UI_INCLUDE_DIR}/controls/Container.hpp
	${CRU_UI_INCLUDE_DIR}/controls/ContentControl.hpp
	${CRU_UI_INCLUDE_DIR}/controls/Control.hpp
	${CRU_UI_INCLUDE_DIR}/controls/FlexLayout.hpp
	${CRU_UI_INCLUDE_DIR}/controls/LayoutControl.hpp
	${CRU_UI_INCLUDE_DIR}/controls/NoChildControl.hpp
	${CRU_UI_INCLUDE_DIR}/controls/StackLayout.hpp
	${CRU_UI_INCLUDE_DIR}/controls/TextBox.hpp
	${CRU_UI_INCLUDE_DIR}/controls/TextBlock.hpp
	${CRU_UI_INCLUDE_DIR}/controls/Window.hpp
	${CRU_UI_INCLUDE_DIR}/events/UiEvent.hpp
	${CRU_UI_INCLUDE_DIR}/helper/ClickDetector.hpp
	${CRU_UI_INCLUDE_DIR}/helper/ShortcutHub.hpp
	${CRU_UI_INCLUDE_DIR}/host/LayoutPaintCycler.hpp
	${CRU_UI_INCLUDE_DIR}/host/WindowHost.hpp
	${CRU_UI_INCLUDE_DIR}/render/Base.hpp
	${CRU_UI_INCLUDE_DIR}/render/BorderRenderObject.hpp
	${CRU_UI_INCLUDE_DIR}/render/CanvasRenderObject.hpp
	${CRU_UI_INCLUDE_DIR}/render/FlexLayoutRenderObject.hpp
	${CRU_UI_INCLUDE_DIR}/render/LayoutHelper.hpp
	${CRU_UI_INCLUDE_DIR}/render/LayoutRenderObject.hpp
	${CRU_UI_INCLUDE_DIR}/render/MeasureRequirement.hpp
	${CRU_UI_INCLUDE_DIR}/render/RenderObject.hpp
	${CRU_UI_INCLUDE_DIR}/render/ScrollRenderObject.hpp
	${CRU_UI_INCLUDE_DIR}/render/StackLayoutRenderObject.hpp
	${CRU_UI_INCLUDE_DIR}/render/TextRenderObject.hpp
)
target_link_libraries(cru_ui PUBLIC cru_platform_gui)
