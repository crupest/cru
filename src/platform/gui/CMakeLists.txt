add_library(cru_platform_gui SHARED
	Keyboard.cpp
	Menu.cpp
	UiApplication.cpp
)
target_link_libraries(cru_platform_gui PUBLIC cru_platform_graphics)
target_compile_definitions(cru_platform_gui PRIVATE CRU_PLATFORM_GUI_EXPORT_API)
