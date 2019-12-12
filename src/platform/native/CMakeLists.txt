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
