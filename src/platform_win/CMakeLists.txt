add_library(cru_platform_win STATIC
	debug.cpp
	exception.cpp
	god_window.cpp
	timer.cpp
	win_application.cpp
	window_class.cpp)
target_include_directories(cru_platform_win PUBLIC ${PROJECT_SOURCE_DIR}/include .)
target_link_libraries(cru_platform_win PRIVATE D3D11 D2d1 DWrite)
target_compile_definitions(cru_platform_win PUBLIC UNICODE _UNICODE) # use unicode
