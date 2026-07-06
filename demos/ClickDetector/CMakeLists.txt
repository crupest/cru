add_executable(CruDemoClickDetector main.cpp)

if(APPLE)
	set_target_properties(CruDemoClickDetector PROPERTIES
		MACOSX_BUNDLE TRUE
		MACOSX_BUNDLE_GUI_IDENTIFIER life.crupest.cru.demo-click-detector
		MACOSX_BUNDLE_BUNDLE_NAME demo-click-detector
	)
endif()

target_add_resources(CruDemoClickDetector cru/ui)
target_link_libraries(CruDemoClickDetector PRIVATE CruDemoBase CruUi)
