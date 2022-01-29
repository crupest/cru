add_executable(cru_theme_builder
	main.cpp
	components/MainWindow.cpp
	components/StyleRuleSetEditor.cpp
)

if(APPLE)
	set_target_properties(cru_theme_builder PROPERTIES
		MACOSX_BUNDLE TRUE
    	MACOSX_BUNDLE_BUNDLE_NAME cru-theme-builder
    	MACOSX_BUNDLE_GUI_IDENTIFIER life.crupest.cru.theme-builder
	)
endif()

target_add_resources(cru_theme_builder cru/ui)
target_link_libraries(cru_theme_builder PRIVATE cru_platform_bootstrap cru_ui)
