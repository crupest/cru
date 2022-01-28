add_library(cru_win_gui SHARED
	TimerManager.hpp
	WindowManager.hpp

	Cursor.cpp
	GodWindow.cpp
	InputMethod.cpp
	Keyboard.cpp
	Resource.cpp
	TimerManager.cpp
	UiApplication.cpp
	Window.cpp
	WindowClass.cpp
	WindowManager.cpp
)
target_link_libraries(cru_win_gui PUBLIC imm32)
target_link_libraries(cru_win_gui PUBLIC cru_win_graphics_direct cru_platform_gui)
target_compile_definitions(cru_win_gui PRIVATE CRU_WIN_GUI_EXPORT_API)
