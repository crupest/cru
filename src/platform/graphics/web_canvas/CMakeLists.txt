add_library(CruPlatformGraphicsWebCanvas
	Factory.cpp
	Resource.cpp
)

target_link_libraries(CruPlatformGraphicsWebCanvas PUBLIC CruPlatformBaseWeb CruPlatformGraphics)

