add_library(cru_win_gui STATIC
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
