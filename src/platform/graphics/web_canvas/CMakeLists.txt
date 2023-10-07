add_library(CruPlatformGraphicsWebCanvas
	Brush.cpp
	Factory.cpp
	Painter.cpp
	Resource.cpp
)

target_link_libraries(CruPlatformGraphicsWebCanvas PUBLIC CruPlatformBaseWeb CruPlatformGraphics)

