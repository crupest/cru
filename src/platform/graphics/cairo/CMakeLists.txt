if (UNIX)
	find_library(LIB_CAIRO cairo REQUIRED)
	find_library(LIB_PANGO NAMES pango pango-1.0 REQUIRED)

	add_library(cru_platform_graphics_cairo SHARED
		CairoGraphicsFactory.cpp
		CairoResource.cpp
	)
	target_compile_definitions(cru_platform_graphics_cairo PRIVATE CRU_PLATFORM_GRAPHICS_CAIRO_EXPORT_API)
	target_link_libraries(cru_platform_graphics_cairo PUBLIC cru_platform_graphics PRIVATE ${LIB_CAIRO} ${LIB_PANGO})
endif()
