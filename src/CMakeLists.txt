add_subdirectory(common)

set(CRU_PLATFORM_BASE_INCLUDE_DIR ${CRU_INCLUDE_DIR}/cru/platform)
add_library(cru_platform_base INTERFACE)
target_sources(cru_platform_base INTERFACE
	${CRU_PLATFORM_BASE_INCLUDE_DIR}/exception.hpp
	${CRU_PLATFORM_BASE_INCLUDE_DIR}/graphic_base.hpp
	${CRU_PLATFORM_BASE_INCLUDE_DIR}/heap_debug.hpp
	${CRU_PLATFORM_BASE_INCLUDE_DIR}/matrix.hpp
	${CRU_PLATFORM_BASE_INCLUDE_DIR}/native_resource.hpp
	${CRU_PLATFORM_BASE_INCLUDE_DIR}/string_util.hpp
)
target_link_libraries(cru_platform_base INTERFACE cru_base)

set(CRU_PLATFORM_GRAPH_INCLUDE_DIR ${CRU_INCLUDE_DIR}/cru/platform/graph)
add_library(cru_platform_graph INTERFACE)
target_sources(cru_platform_graph INTERFACE
	${CRU_PLATFORM_GRAPH_INCLUDE_DIR}/brush.hpp
	${CRU_PLATFORM_GRAPH_INCLUDE_DIR}/font.hpp
	${CRU_PLATFORM_GRAPH_INCLUDE_DIR}/geometry.hpp
	${CRU_PLATFORM_GRAPH_INCLUDE_DIR}/graph_factory.hpp
	${CRU_PLATFORM_GRAPH_INCLUDE_DIR}/painter.hpp
	${CRU_PLATFORM_GRAPH_INCLUDE_DIR}/text_layout.hpp
	${CRU_PLATFORM_GRAPH_INCLUDE_DIR}/util/painter_util.hpp
)
target_link_libraries(cru_platform_graph INTERFACE cru_platform_base)

set(CRU_PLATFORM_NATIVE_INCLUDE_DIR ${CRU_INCLUDE_DIR}/cru/platform/native)
add_library(cru_platform_native INTERFACE)
target_sources(cru_platform_native INTERFACE
	${CRU_PLATFORM_NATIVE_INCLUDE_DIR}/basic_types.hpp
	${CRU_PLATFORM_NATIVE_INCLUDE_DIR}/cursor.hpp
	${CRU_PLATFORM_NATIVE_INCLUDE_DIR}/native_event.hpp
	${CRU_PLATFORM_NATIVE_INCLUDE_DIR}/native_window.hpp
	${CRU_PLATFORM_NATIVE_INCLUDE_DIR}/ui_application.hpp
)
target_link_libraries(cru_platform_native INTERFACE cru_platform_graph)

if(WIN32)
  add_subdirectory(win)
endif()

add_subdirectory(ui)

add_executable(demo main.cpp)
target_link_libraries(demo PRIVATE cru_ui)
