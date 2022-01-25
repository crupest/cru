add_executable(demo_input_method main.cpp)

if(APPLE)
	set_target_properties(demo_input_method PROPERTIES
		MACOSX_BUNDLE TRUE
    	MACOSX_BUNDLE_BUNDLE_NAME demo-input-method
    	MACOSX_BUNDLE_GUI_IDENTIFIER life.crupest.cru.demo-input-method
	)
endif()

target_link_libraries(demo_input_method PRIVATE cru_demo_base)

