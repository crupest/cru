if (UNIX)
	set(LIB_ARCH_DIR ${CMAKE_SYSTEM_PREFIX_PATH})
	list(TRANSFORM LIB_ARCH_DIR APPEND "/lib/${CMAKE_LIBRARY_ARCHITECTURE}")
	message("${LIB_ARCH_DIR}")

	find_library(LIB_CAIRO cairo REQUIRED)
	find_library(LIB_GOBJECT NAMES gobject gobject-2.0 REQUIRED)
	find_library(LIB_PANGO NAMES pango pango-1.0 REQUIRED)
	find_path(GLIB_HEADER_DIR NAMES glib.h PATH_SUFFIXES glib glib-2.0 REQUIRED)
	find_path(GLIBCONFIG_HEADER_DIR NAMES glibconfig.h HINTS ${LIB_ARCH_DIR} PATH_SUFFIXES glib glib/include glib-2.0 glib-2.0/include REQUIRED)
	find_path(HARFBUZZ_HEADER_DIR NAMES hb.h PATH_SUFFIXES harfbuzz REQUIRED)
	find_path(PANGO_HEADER_DIR NAMES pango PATH_SUFFIXES pango-1.0 REQUIRED)
	
	add_library(CruPlatformGraphicsCairo SHARED
	    Base.cpp
		CairoBrush.cpp
		CairoGeometry.cpp
		CairoGraphicsFactory.cpp
		CairoResource.cpp
		PangoFont.cpp
		PangoTextLayout.cpp
	)
	target_compile_definitions(CruPlatformGraphicsCairo PRIVATE CRU_PLATFORM_GRAPHICS_CAIRO_EXPORT_API)
	target_link_libraries(CruPlatformGraphicsCairo PUBLIC CruPlatformGraphics PUBLIC ${LIB_GOBJECT} ${LIB_CAIRO} ${LIB_PANGO})
	target_include_directories(CruPlatformGraphicsCairo PUBLIC ${GLIB_HEADER_DIR} ${GLIBCONFIG_HEADER_DIR} ${HARFBUZZ_HEADER_DIR} ${PANGO_HEADER_DIR})
endif()
