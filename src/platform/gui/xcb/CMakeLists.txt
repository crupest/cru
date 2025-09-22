find_library(LIBRARY_CAIRO cairo REQUIRED)
find_library(LIBRARY_XCB xcb REQUIRED)
add_library(CruPlatformGuiXcb
	Cursor.cpp
	Keyboard.cpp
	UiApplication.cpp
	Window.cpp
)
target_link_libraries(CruPlatformGuiXcb PUBLIC
	CruPlatformGui CruPlatformGraphicsCairo
	${LIBRARY_XCB} ${LIBRARY_CAIRO}
)
