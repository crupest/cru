set(CRU_UI_INCLUDE_DIR ${CRU_INCLUDE_DIR}/cru/ui)

add_library(cru_ui STATIC
	Helper.cpp
	ThemeManager.cpp
	UiManager.cpp
	components/Component.cpp
	components/Menu.cpp
	controls/Button.cpp
	controls/Container.cpp
	controls/ContentControl.cpp
	controls/Control.cpp
	controls/FlexLayout.cpp
	controls/LayoutControl.cpp
	controls/NoChildControl.cpp
	controls/Popup.cpp
	controls/RootControl.cpp
	controls/ScrollView.cpp
	controls/StackLayout.cpp
	controls/TextBlock.cpp
	controls/TextBox.cpp
	controls/TextHostControlService.cpp
	controls/Window.cpp
	events/MouseEventArgs.cpp
	helper/ClickDetector.cpp
	helper/ShortcutHub.cpp
	host/LayoutPaintCycler.cpp
	host/WindowHost.cpp
	render/BorderRenderObject.cpp
	render/CanvasRenderObject.cpp
	render/FlexLayoutRenderObject.cpp
	render/LayoutHelper.cpp
	render/RenderObject.cpp
	render/ScrollBar.cpp
	render/ScrollRenderObject.cpp
	render/StackLayoutRenderObject.cpp
	render/TextRenderObject.cpp
	style/Condition.cpp
	style/Styler.cpp
	style/StyleRule.cpp
	style/StyleRuleSet.cpp
	xml/XmlMapper.cpp
	xml/XmlMapperRegistry.cpp
)
target_link_libraries(cru_ui PUBLIC cru_platform_gui)
