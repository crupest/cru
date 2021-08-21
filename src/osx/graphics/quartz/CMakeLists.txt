set(CRU_OSX_GRAPHICS_NATIVE_INCLUDE_DIR ${CRU_INCLUDE_DIR}/cru/osx/graphics/quartz)

add_library(cru_osx_graphics_quartz SHARED
	Convert.cpp
	Factory.cpp
	Painter.cpp
	Resource.cpp
)
target_sources(cru_osx_graphics_quartz PUBLIC
	${CRU_OSX_GRAPHICS_NATIVE_INCLUDE_DIR}/Convert.hpp
	${CRU_OSX_GRAPHICS_NATIVE_INCLUDE_DIR}/Factory.hpp
	${CRU_OSX_GRAPHICS_NATIVE_INCLUDE_DIR}/Painter.hpp
	${CRU_OSX_GRAPHICS_NATIVE_INCLUDE_DIR}/Resource.hpp
)
target_link_libraries(cru_osx_graphics_quartz PUBLIC cocoa)
target_link_libraries(cru_osx_graphics_quartz PUBLIC cru_osx_base cru_platform_graphics)

