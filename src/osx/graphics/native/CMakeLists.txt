set(CRU_OSX_GRAPHICS_NATIVE_INCLUDE_DIR ${CRU_INCLUDE_DIR}/cru/osx/graphics/native)

add_library(cru_win_graphics_direct SHARED
	Factory.cpp
)
target_sources(cru_win_graphics_direct PUBLIC
	${CRU_OSX_GRAPHICS_NATIVE_INCLUDE_DIR}/Factory.hpp
)
target_link_libraries(cru_win_graphics_direct PUBLIC cocoa)
target_link_libraries(cru_win_graphics_direct PUBLIC cru_osx_base cru_platform_graphics)

