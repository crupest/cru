add_executable(demo_main main.cpp)

if(APPLE)
	set_target_properties(demo_main PROPERTIES
		MACOSX_BUNDLE TRUE
    	MACOSX_BUNDLE_BUNDLE_NAME demo-main
    	MACOSX_BUNDLE_GUI_IDENTIFIER life.crupest.cru.demo-main
	)
endif()

target_link_libraries(demo_main PRIVATE cru_demo_base cru_ui)
