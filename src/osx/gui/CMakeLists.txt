set(CRU_OSX_GUI_INCLUDE_DIR ${CRU_INCLUDE_DIR}/cru/osx/gui)

add_library(cru_osx_gui SHARED
	Keyboard.mm
	Resource.cpp
	UiApplication.mm
	Window.mm
)

target_sources(cru_osx_gui PUBLIC
	${CRU_OSX_GUI_INCLUDE_DIR}/Keyboard.hpp
	${CRU_OSX_GUI_INCLUDE_DIR}/Resource.hpp
	${CRU_OSX_GUI_INCLUDE_DIR}/UiApplication.hpp
	${CRU_OSX_GUI_INCLUDE_DIR}/Window.hpp
)

find_library(APPKIT AppKit REQUIRED)

target_link_libraries(cru_osx_gui PUBLIC cru_platform_gui cru_osx_graphics_quartz ${APPKIT})
