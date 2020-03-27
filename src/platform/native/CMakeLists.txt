set(CRU_PLATFORM_NATIVE_INCLUDE_DIR ${CRU_INCLUDE_DIR}/cru/platform/native)
add_library(cru_platform_native STATIC
	ui_application.cpp
)
target_sources(cru_platform_native PUBLIC
	${CRU_PLATFORM_NATIVE_INCLUDE_DIR}/base.hpp
	${CRU_PLATFORM_NATIVE_INCLUDE_DIR}/cursor.hpp
	${CRU_PLATFORM_NATIVE_INCLUDE_DIR}/key_code.hpp
	${CRU_PLATFORM_NATIVE_INCLUDE_DIR}/window.hpp
	${CRU_PLATFORM_NATIVE_INCLUDE_DIR}/ui_application.hpp
)
target_link_libraries(cru_platform_native PUBLIC cru_platform_graph)
