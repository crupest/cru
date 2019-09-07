set(CRU_WIN_NATIVE_INCLUDE_DIR ${CRU_INCLUDE_DIR}/cru/win/native)

add_library(cru_win_native STATIC
	dpi_util.hpp
	god_window_message.hpp
	timer.hpp
	window_d2d_painter.hpp
	window_manager.hpp

	cursor.cpp
	god_window.cpp
	native_window.cpp
	timer.cpp
	ui_application.cpp
	window_class.cpp
	window_d2d_painter.cpp
	window_manager.cpp
	window_render_target.cpp
)
target_sources(cru_win_native PUBLIC
	${CRU_WIN_NATIVE_INCLUDE_DIR}/cursor.hpp
	${CRU_WIN_NATIVE_INCLUDE_DIR}/exception.hpp
	${CRU_WIN_NATIVE_INCLUDE_DIR}/god_window.hpp
	${CRU_WIN_NATIVE_INCLUDE_DIR}/native_window.hpp
	${CRU_WIN_NATIVE_INCLUDE_DIR}/platform_id.hpp
	${CRU_WIN_NATIVE_INCLUDE_DIR}/ui_application.hpp
	${CRU_WIN_NATIVE_INCLUDE_DIR}/window_class.hpp
	${CRU_WIN_NATIVE_INCLUDE_DIR}/window_native_message_event_args.hpp
	${CRU_WIN_NATIVE_INCLUDE_DIR}/window_render_target.hpp
)
target_link_libraries(cru_win_native PUBLIC cru_win_graph_direct)
