add_library(cru_win_native STATIC
	god_window.cpp
	timer.cpp
	win_application.cpp
	win_native_window.cpp
	window_class.cpp
	window_manager.cpp
	window_painter.cpp
	window_render_target.cpp)
target_link_libraries(cru_win_native PUBLIC cru_win_graph)
