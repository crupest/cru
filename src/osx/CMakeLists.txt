set(CRU_OSX_BASE_INCLUDE_DIR ${CRU_INCLUDE_DIR}/cru/osx)

add_library(cru_osx_base SHARED
	Exception.cpp
)

target_sources(cru_osx_base PUBLIC
	${CRU_OSX_BASE_INCLUDE_DIR}/Exception.hpp
)

add_subdirectory(graphics)
