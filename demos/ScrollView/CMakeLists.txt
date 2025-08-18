add_executable(CruDemoScrollView main.cpp)

if(APPLE)
	set_target_properties(CruDemoScrollView PROPERTIES
		MACOSX_BUNDLE TRUE
    	MACOSX_BUNDLE_GUI_IDENTIFIER life.crupest.cru.demo-scroll-view
    	MACOSX_BUNDLE_BUNDLE_NAME demo-scroll-view
	)
endif()

target_link_libraries(CruDemoScrollView PRIVATE CruDemoBase CruUi)
