
if(NOT APPLE)
	add_executable(demo_input_method main.cpp)
else()
	add_executable(demo_input_method MACOSX_BUNDLE main.cpp)
	set_target_properties(demo_input_method PROPERTIES
		BUNDLE TRUE
    	MACOSX_BUNDLE_GUI_IDENTIFIER life.crupest.cru.demo_input_method
    	MACOSX_BUNDLE_BUNDLE_NAME demo_input_method
	)
endif()

target_link_libraries(demo_input_method PRIVATE cru_demo_base)

