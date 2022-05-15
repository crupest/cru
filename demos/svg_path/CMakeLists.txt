add_executable(CruDemoSvgPath main.cpp)

if(APPLE)
	set_target_properties(CruDemoSvgPath PROPERTIES
		MACOSX_BUNDLE TRUE
    	MACOSX_BUNDLE_BUNDLE_NAME cru-demo-svg-path
    	MACOSX_BUNDLE_GUI_IDENTIFIER life.crupest.demo-svg-path
	)
endif()

target_link_libraries(CruDemoSvgPath PRIVATE cru_demo_base)

