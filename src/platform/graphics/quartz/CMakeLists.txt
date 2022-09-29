add_library(CruPlatformGraphicsQuartz
	Brush.cpp
	Convert.cpp
	Factory.cpp
	Font.cpp
	Geometry.cpp
	Image.cpp
	ImageFactory.cpp
	Painter.cpp
	Resource.cpp
	TextLayout.cpp
)

find_library(CORE_GRAPHICS CoreGraphics REQUIRED)
find_library(CORE_TEXT CoreText REQUIRED)
find_library(IMAGE_IO ImageIO REQUIRED)

target_link_libraries(CruPlatformGraphicsQuartz PUBLIC ${CORE_GRAPHICS} ${CORE_TEXT} ${IMAGE_IO})
target_link_libraries(CruPlatformGraphicsQuartz PUBLIC CruPlatformBaseOsx CruPlatformGraphics)
