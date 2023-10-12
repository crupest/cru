add_library(CruPlatformGraphicsWebCanvas
	Brush.cpp
	Factory.cpp
	Geometry.cpp
	Matrix.cpp
	Painter.cpp
	Resource.cpp
	WebCanvasRef.cpp
)

target_link_libraries(CruPlatformGraphicsWebCanvas PUBLIC CruPlatformBaseWeb CruPlatformGraphics)

