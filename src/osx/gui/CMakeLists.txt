add_library(cru_osx_gui SHARED
	Clipboard.mm
	Cursor.mm
	InputMethod.mm
	Keyboard.mm
	Menu.mm
	Resource.cpp
	UiApplication.mm
	Window.mm
)

find_library(APPKIT AppKit REQUIRED)
find_library(UNIFORMTYPEIDENTIFIERS UniformTypeIdentifiers REQUIRED)

target_link_libraries(cru_osx_gui PUBLIC cru_platform_gui cru_osx_graphics_quartz ${APPKIT} ${UNIFORMTYPEIDENTIFIERS})
