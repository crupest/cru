set(CRU_OSX_GUI_INCLUDE_DIR ${CRU_INCLUDE_DIR}/cru/osx/gui)

add_library(cru_osx_gui SHARED
	Resource.cpp
	UiApplication.cpp
	Window.cpp
)

target_sources(cru_osx_gui PUBLIC
	${CRU_OSX_GUI_INCLUDE_DIR}/Resource.hpp
	${CRU_OSX_GUI_INCLUDE_DIR}/UiApplication.hpp
	${CRU_OSX_GUI_INCLUDE_DIR}/Window.hpp
)

target_link_libraries(cru_osx_gui PUBLIC cru_platform_gui cru_osx_graphics_quartz)
