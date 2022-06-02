if (UNIX)
	find_library(LIB_CAIRO cairo REQUIRED)
	find_library(LIB_PANGO NAMES pango pango-1.0 REQUIRED)

	add_library(CruPlatformGraphicsCairo SHARED
	    Base.cpp
		CairoBrush.cpp
		CairoGeometry.cpp
		CairoGraphicsFactory.cpp
		CairoResource.cpp
		PangoFont.cpp
	)
	target_compile_definitions(CruPlatformGraphicsCairo PRIVATE CRU_PLATFORM_GRAPHICS_CAIRO_EXPORT_API)
	target_link_libraries(CruPlatformGraphicsCairo PUBLIC CruPlatformGraphics PUBLIC ${LIB_CAIRO} ${LIB_PANGO})
endif()
