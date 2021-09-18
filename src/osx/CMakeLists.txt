set(CRU_OSX_BASE_INCLUDE_DIR ${CRU_INCLUDE_DIR}/cru/osx)

add_library(cru_osx_base SHARED
	Exception.cpp
	Resource.cpp
)

target_sources(cru_osx_base PUBLIC
	${CRU_OSX_BASE_INCLUDE_DIR}/Exception.hpp
	${CRU_OSX_BASE_INCLUDE_DIR}/Resource.hpp
)

target_link_libraries(cru_osx_base PUBLIC cru_platform_base)

add_subdirectory(graphics)
add_subdirectory(gui)
