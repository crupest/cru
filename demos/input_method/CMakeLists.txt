add_executable(CruDemoInputMethod main.cpp)

if(APPLE)
	set_target_properties(CruDemoInputMethod PROPERTIES
		MACOSX_BUNDLE TRUE
    	MACOSX_BUNDLE_BUNDLE_NAME demo-input-method
    	MACOSX_BUNDLE_GUI_IDENTIFIER life.crupest.cru.demo-input-method
	)
endif()

target_link_libraries(CruDemoInputMethod PRIVATE cru_demo_base)

