add_executable(CruDemoMain main.cpp)

if(APPLE)
	set_target_properties(CruDemoMain PROPERTIES
		MACOSX_BUNDLE TRUE
    	MACOSX_BUNDLE_BUNDLE_NAME demo-main
    	MACOSX_BUNDLE_GUI_IDENTIFIER life.crupest.cru.demo-main
	)
endif()

target_link_libraries(CruDemoMain PRIVATE CruDemoBase CruUi)
