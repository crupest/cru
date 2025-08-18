add_executable(CruThemeBuilder
	main.cpp
	components/Common.cpp
	components/Editor.cpp
	components/HeadBodyEditor.cpp
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
	components/stylers/FontStylerEditor.cpp
	components/stylers/MarginStylerEditor.cpp
	components/stylers/PaddingStylerEditor.cpp
	components/stylers/PreferredSizeStylerEditor.cpp
	components/stylers/StylerEditor.cpp
)

if(APPLE)
	set_target_properties(CruThemeBuilder PROPERTIES
		MACOSX_BUNDLE TRUE
    	MACOSX_BUNDLE_BUNDLE_NAME cru-theme-builder
    	MACOSX_BUNDLE_GUI_IDENTIFIER life.crupest.cru.theme-builder
	)
endif()

target_link_libraries(CruThemeBuilder PRIVATE CruPlatformBootstrap CruUi)
