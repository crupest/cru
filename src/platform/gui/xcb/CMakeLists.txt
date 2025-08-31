find_library(LIBRARY_CAIRO cairo REQUIRED)
find_library(LIBRARY_XCB xcb REQUIRED)
add_library(CruPlatformGuiX11
	UiApplication.cpp
	Window.cpp
)
target_link_libraries(CruPlatformGuiX11 PUBLIC CruPlatformGui ${LIBRARY_XCB} ${LIBRARY_CAIRO})

