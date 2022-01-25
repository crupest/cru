add_executable(demo_scroll_view main.cpp)

if(APPLE)
	set_target_properties(demo_scroll_view PROPERTIES
		MACOSX_BUNDLE TRUE
    	MACOSX_BUNDLE_GUI_IDENTIFIER life.crupest.cru.demo-scroll-view
    	MACOSX_BUNDLE_BUNDLE_NAME demo-scroll-view
	)
endif()

target_link_libraries(demo_scroll_view PRIVATE cru_demo_base cru_ui)
