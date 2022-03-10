add_executable(cru_theme_builder
	main.cpp
	components/Editor.cpp
	components/MainWindow.cpp
	components/StyleRuleEditor.cpp
	components/StyleRuleSetEditor.cpp
	components/conditions/CheckedConditionEditor.cpp
	components/conditions/ClickStateConditionEditor.cpp
	components/conditions/CompoundConditionEditor.cpp
	components/conditions/ConditionEditor.cpp
	components/conditions/FocusConditionEditor.cpp
	components/conditions/NoConditionEditor.cpp
	components/properties/CheckBoxPropertyEditor.cpp
	components/properties/ColorPropertyEditor.cpp
	components/properties/CornerRadiusPropertyEditor.cpp
	components/properties/FontPropertyEditor.cpp
	components/properties/MeasureLengthPropertyEditor.cpp
	components/properties/PointPropertyEditor.cpp
	components/properties/SelectPropertyEditor.cpp
	components/properties/TextPropertyEditor.cpp
	components/properties/ThicknessPropertyEditor.cpp
	components/stylers/BorderStylerEditor.cpp
	components/stylers/CompoundStylerEditor.cpp
	components/stylers/ContentBrushStylerEditor.cpp
	components/stylers/CursorStylerEditor.cpp
	components/stylers/MarginStylerEditor.cpp
	components/stylers/PaddingStylerEditor.cpp
	components/stylers/PreferredSizeStylerEditor.cpp
	components/stylers/StylerEditor.cpp
)

if(APPLE)
	set_target_properties(cru_theme_builder PROPERTIES
		MACOSX_BUNDLE TRUE
    	MACOSX_BUNDLE_BUNDLE_NAME cru-theme-builder
    	MACOSX_BUNDLE_GUI_IDENTIFIER life.crupest.cru.theme-builder
	)
endif()

target_add_resources(cru_theme_builder cru/ui)
target_add_resources(cru_theme_builder cru/theme_builder)
target_link_libraries(cru_theme_builder PRIVATE cru_platform_bootstrap cru_ui)
