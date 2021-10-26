if(NOT APPLE)
	add_executable(demo_scroll_view main.cpp)
else()
	add_executable(demo_scroll_view MACOSX_BUNDLE main.cpp)
	set_target_properties(demo_scroll_view PROPERTIES
		BUNDLE TRUE
    	MACOSX_BUNDLE_GUI_IDENTIFIER life.crupest.cru.demo-scroll-view
    	MACOSX_BUNDLE_BUNDLE_NAME demo-scroll-view
	)
endif()

target_link_libraries(demo_scroll_view PRIVATE cru_demo_base cru_ui)
