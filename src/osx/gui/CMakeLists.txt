add_library(CruPlatformGuiOsx SHARED
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

target_link_libraries(CruPlatformGuiOsx PUBLIC CruPlatformGui CruPlatformGraphicsQuartz ${APPKIT} ${UNIFORMTYPEIDENTIFIERS})
