find_library(LIBRARY_CAIRO cairo REQUIRED)
find_library(LIBRARY_XCB xcb REQUIRED)
find_library(LIBRARY_XCB_CURSOR xcb-cursor REQUIRED)
find_library(LIBRARY_XCB_IMDKIT xcb-imdkit REQUIRED)
add_library(CruPlatformGuiXcb
	Cursor.cpp
	InputMethod.cpp
	Keyboard.cpp
	UiApplication.cpp
	Window.cpp
)
target_link_libraries(CruPlatformGuiXcb PUBLIC
	CruPlatformGui CruPlatformGraphicsCairo
	${LIBRARY_XCB} ${LIBRARY_XCB_CURSOR} ${LIBRARY_XCB_IMDKIT} ${LIBRARY_CAIRO}
)
