add_library(cru_win_base STATIC
	DebugLogger.hpp
	StdOutLogger.hpp

	Exception.cpp
	HeapDebug.cpp
)
target_compile_definitions(cru_win_base PUBLIC UNICODE _UNICODE) # use unicode
target_link_libraries(cru_win_base PUBLIC cru_base)

add_subdirectory(graphics)
add_subdirectory(gui)
