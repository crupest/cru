set(CRU_PLATFORM_GUI_INCLUDE_DIR ${CRU_INCLUDE_DIR}/cru/platform/gui)
add_library(cru_platform_gui STATIC
	Keyboard.cpp
	UiApplication.cpp
)
target_sources(cru_platform_gui PUBLIC
	${CRU_PLATFORM_GUI_INCLUDE_DIR}/Base.hpp
	${CRU_PLATFORM_GUI_INCLUDE_DIR}/Cursor.hpp
	${CRU_PLATFORM_GUI_INCLUDE_DIR}/InputMethod.hpp
	${CRU_PLATFORM_GUI_INCLUDE_DIR}/Keyboard.hpp
	${CRU_PLATFORM_GUI_INCLUDE_DIR}/Window.hpp
	${CRU_PLATFORM_GUI_INCLUDE_DIR}/UiApplication.hpp
)
target_link_libraries(cru_platform_gui PUBLIC cru_platform_graphics)
