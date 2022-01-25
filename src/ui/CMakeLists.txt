add_library(cru_ui SHARED
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
	mapper/BorderStyleMapper.cpp
	mapper/ColorMapper.cpp
	mapper/CornerRadiusMapper.cpp
	mapper/CursorMapper.cpp
	mapper/Mapper.cpp
	mapper/MapperRegistry.cpp
	mapper/PointMapper.cpp
	mapper/SizeMapper.cpp
	mapper/ThicknessMapper.cpp
	mapper/style/AndConditionMapper.cpp
	mapper/style/BorderStylerMapper.cpp
	mapper/style/ClickStateConditionMapper.cpp
	mapper/style/CursorStylerMapper.cpp
	mapper/style/FocusConditionMapper.cpp
	mapper/style/HoverConditionMapper.cpp
	mapper/style/NoConditionMapper.cpp
	mapper/style/OrConditionMapper.cpp
	mapper/style/StyleRuleMapper.cpp
	mapper/style/StyleRuleSetMapper.cpp
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
)
target_compile_definitions(cru_ui PRIVATE CRU_UI_EXPORT_API)
target_link_libraries(cru_ui PUBLIC cru_platform_gui cru_xml)

file(GLOB_RECURSE CRU_UI_RES_SOURCES "${CRU_ASSETS_DIR}/cru/ui/*")
target_sources(cru_ui PUBLIC ${CRU_UI_RES_SOURCES})
foreach (CRU_UI_RES_FILE ${CRU_UI_RES_SOURCES})
	file(RELATIVE_PATH CRU_UI_RES_PATH ${CRU_ASSETS_DIR} ${CRU_UI_RES_FILE})
	set_property(SOURCE ${CRU_UI_RES_FILE} PROPERTY MACOSX_PACKAGE_LOCATION "Resources/${CRU_UI_RES_PATH}")
endforeach(CRU_UI_RES_FILE)
