set(CRU_WIN_NATIVE_INCLUDE_DIR ${CRU_INCLUDE_DIR}/cru/win/native)

add_library(cru_win_native STATIC
	DpiUtil.hpp
	TimerManager.hpp
	WindowD2DPainter.hpp
	WindowManager.hpp

	Cursor.cpp
	GodWindow.cpp
	InputMethod.cpp
	Keyboard.cpp
	TimerManager.cpp
	UiApplication.cpp
	Window.cpp
	WindowClass.cpp
	WindowD2DPainter.cpp
	WindowManager.cpp
	WindowRenderTarget.cpp
)
target_sources(cru_win_native PUBLIC
	${CRU_WIN_NATIVE_INCLUDE_DIR}/Cursor.hpp
	${CRU_WIN_NATIVE_INCLUDE_DIR}/Exception.hpp
	${CRU_WIN_NATIVE_INCLUDE_DIR}/Base.hpp
	${CRU_WIN_NATIVE_INCLUDE_DIR}/GodWindow.hpp
	${CRU_WIN_NATIVE_INCLUDE_DIR}/InputMethod.hpp
	${CRU_WIN_NATIVE_INCLUDE_DIR}/Keyboard.hpp
	${CRU_WIN_NATIVE_INCLUDE_DIR}/Resource.hpp
	${CRU_WIN_NATIVE_INCLUDE_DIR}/UiApplication.hpp
	${CRU_WIN_NATIVE_INCLUDE_DIR}/Window.hpp
	${CRU_WIN_NATIVE_INCLUDE_DIR}/WindowClass.hpp
	${CRU_WIN_NATIVE_INCLUDE_DIR}/WindowNativeMessageEventArgs.hpp
	${CRU_WIN_NATIVE_INCLUDE_DIR}/WindowRenderTarget.hpp
)
target_link_libraries(cru_win_native PUBLIC imm32)
target_link_libraries(cru_win_native PUBLIC cru_win_graph_direct cru_platform_native)
