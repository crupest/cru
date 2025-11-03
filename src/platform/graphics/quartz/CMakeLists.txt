add_library(CruPlatformGraphicsQuartz
	Base.cpp
	Brush.cpp
	Factory.cpp
	Font.cpp
	Geometry.cpp
	Image.cpp
	ImageFactory.cpp
	Painter.cpp
	TextLayout.cpp
)

find_library(FOUNDATION Foundation REQUIRED)
find_library(CORE_FOUNDATION CoreFoundation REQUIRED)
find_library(CORE_GRAPHICS CoreGraphics REQUIRED)
find_library(CORE_TEXT CoreText REQUIRED)
find_library(IMAGE_IO ImageIO REQUIRED)

target_link_libraries(CruPlatformGraphicsQuartz PUBLIC ${FOUNDATION} ${CORE_FOUNDATION} ${CORE_GRAPHICS} ${CORE_TEXT} ${IMAGE_IO})
target_link_libraries(CruPlatformGraphicsQuartz PUBLIC CruPlatformGraphics)
