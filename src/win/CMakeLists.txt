set(CRU_WIN_BASE_INCLUDE_DIR ${CRU_INCLUDE_DIR}/cru/win/)

add_library(cru_win_base STATIC
	debug.cpp
	exception.cpp
	heap_debug.cpp
	string_util.cpp
)
target_sources(cru_win_base PUBLIC
	${CRU_WIN_BASE_INCLUDE_DIR}/exception.hpp
	${CRU_WIN_BASE_INCLUDE_DIR}/win_pre_config.hpp
)
target_compile_definitions(cru_win_base PUBLIC UNICODE _UNICODE) # use unicode
target_include_directories(cru_win_base PUBLIC .)
target_link_libraries(cru_win_base PUBLIC cru_base)

add_subdirectory(graph)
add_subdirectory(native)
