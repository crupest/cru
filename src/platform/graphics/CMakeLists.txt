set(CRU_PLATFORM_GRAPHICS_INCLUDE_DIR ${CRU_INCLUDE_DIR}/cru/platform/graphics)
add_library(cru_platform_graphics SHARED
	NullPainter.cpp
)
target_sources(cru_platform_graphics PUBLIC
	${CRU_PLATFORM_GRAPHICS_INCLUDE_DIR}/Base.hpp
	${CRU_PLATFORM_GRAPHICS_INCLUDE_DIR}/Brush.hpp
	${CRU_PLATFORM_GRAPHICS_INCLUDE_DIR}/Font.hpp
	${CRU_PLATFORM_GRAPHICS_INCLUDE_DIR}/Geometry.hpp
	${CRU_PLATFORM_GRAPHICS_INCLUDE_DIR}/NullPainter.hpp
	${CRU_PLATFORM_GRAPHICS_INCLUDE_DIR}/Factory.hpp
	${CRU_PLATFORM_GRAPHICS_INCLUDE_DIR}/Resource.hpp
	${CRU_PLATFORM_GRAPHICS_INCLUDE_DIR}/Painter.hpp
	${CRU_PLATFORM_GRAPHICS_INCLUDE_DIR}/TextLayout.hpp
	${CRU_PLATFORM_GRAPHICS_INCLUDE_DIR}/util/Painter.hpp
)
target_link_libraries(cru_platform_graphics PUBLIC cru_platform_base)
