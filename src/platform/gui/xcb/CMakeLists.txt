find_library(LIBRARY_XCB xcb REQUIRED)
find_library(LIBRARY_XCB_CURSOR xcb-cursor REQUIRED)
find_library(LIBRARY_XCB_IMDKIT xcb-imdkit REQUIRED)
find_library(LIBRARY_XKBCOMMON xkbcommon REQUIRED)
find_library(LIBRARY_XKBCOMMON_X11 xkbcommon-x11 REQUIRED)
add_library(CruPlatformGuiXcb
	Clipboard.cpp
	Cursor.cpp
	Input.cpp
	InputMethod.cpp
	UiApplication.cpp
	Window.cpp
)
target_link_libraries(CruPlatformGuiXcb PUBLIC
	CruPlatformGui CruPlatformGraphicsCairo
	"${LIBRARY_XCB}" "${LIBRARY_XCB_CURSOR}" "${LIBRARY_XCB_IMDKIT}"
	"${LIBRARY_XKBCOMMON}" "${LIBRARY_XKBCOMMON_X11}"
)
