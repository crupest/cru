if (UNIX)
	find_library(LIB_CAIRO cairo REQUIRED)
	find_library(LIB_PANGO NAMES pango pango-1.0 REQUIRED)

	add_library(CruPlatformGraphicsCairo SHARED
		CairoGraphicsFactory.cpp
		CairoResource.cpp
	)
	target_compile_definitions(CruPlatformGraphicsCairo PRIVATE CRU_PLATFORM_GRAPHICS_CAIRO_EXPORT_API)
	target_link_libraries(CruPlatformGraphicsCairo PUBLIC CruPlatformGraphics PRIVATE ${LIB_CAIRO} ${LIB_PANGO})
endif()
