add_library(cru_osx_graphics_quartz SHARED
	Brush.cpp
	Convert.cpp
	Factory.cpp
	Font.cpp
	Geometry.cpp
	Painter.cpp
	Resource.cpp
	TextLayout.cpp
)

find_library(CORE_GRAPHICS CoreGraphics REQUIRED)
find_library(CORE_TEXT CoreText REQUIRED)

target_link_libraries(cru_osx_graphics_quartz PUBLIC ${CORE_GRAPHICS} ${CORE_TEXT})
target_link_libraries(cru_osx_graphics_quartz PUBLIC cru_osx_base cru_platform_graphics)
