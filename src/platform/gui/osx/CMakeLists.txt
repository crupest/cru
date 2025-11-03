add_library(CruPlatformGuiOsx
	Base.cpp
	Clipboard.mm
	Cursor.mm
	InputMethod.mm
	Keyboard.mm
	Menu.mm
	UiApplication.mm
	Window.mm
)

find_library(FOUNDATION Foundation REQUIRED)
find_library(CORE_FOUNDATION CoreFoundation REQUIRED)
find_library(APPKIT AppKit REQUIRED)
find_library(UNIFORMTYPEIDENTIFIERS UniformTypeIdentifiers REQUIRED)

target_link_libraries(CruPlatformGuiOsx PUBLIC CruPlatformGui CruPlatformGraphicsQuartz ${FOUNDATION} ${CORE_FOUNDATION} ${APPKIT} ${UNIFORMTYPEIDENTIFIERS})
