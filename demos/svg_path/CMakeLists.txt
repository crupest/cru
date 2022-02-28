add_executable(cru_demo_svg_path main.cpp)

if(APPLE)
	set_target_properties(cru_demo_svg_path PROPERTIES
		MACOSX_BUNDLE TRUE
    	MACOSX_BUNDLE_BUNDLE_NAME cru-demo-svg-path
    	MACOSX_BUNDLE_GUI_IDENTIFIER life.crupest.demo-svg-path
	)
endif()

target_link_libraries(cru_demo_svg_path PRIVATE cru_demo_base)

