if(NOT APPLE)
	add_executable(demo_main main.cpp)
else()
	add_executable(demo_main MACOSX_BUNDLE main.cpp)
	set_target_properties(demo_main PROPERTIES
		BUNDLE TRUE
    	MACOSX_BUNDLE_GUI_IDENTIFIER life.crupest.cru.demo-main
    	MACOSX_BUNDLE_BUNDLE_NAME demo-main
	)
endif()

target_link_libraries(demo_main PRIVATE cru_demo_base cru_ui)
