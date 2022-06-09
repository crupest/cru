if (UNIX)
	foreach(PP IN LISTS CMAKE_SYSTEM_PREFIX_PATH)
		cmake_path(APPEND PP "lib" ${CMAKE_LIBRARY_ARCHITECTURE})
		list(APPEND LIB_ARCH_DIR ${PP})
	endforeach()

	find_library(LIB_CAIRO cairo REQUIRED)
	find_library(LIB_GOBJECT NAMES gobject gobject-2.0 REQUIRED)
	find_library(LIB_PANGO NAMES pango pango-1.0 REQUIRED)
	find_library(LIB_PANGOCAIRO NAMES pangocairo pangocairo-1.0 REQUIRED)
	find_path(CAIRO_HEADER_DIR NAMES cairo.h PATH_SUFFIXES cairo REQUIRED)
	find_path(GLIB_HEADER_DIR NAMES glib.h PATH_SUFFIXES glib glib-2.0 REQUIRED)
	find_path(GLIBCONFIG_HEADER_DIR NAMES glibconfig.h HINTS ${LIB_ARCH_DIR} PATH_SUFFIXES glib glib/include glib-2.0 glib-2.0/include REQUIRED)
	find_path(HARFBUZZ_HEADER_DIR NAMES hb.h PATH_SUFFIXES harfbuzz REQUIRED)
	find_path(PANGO_HEADER_DIR NAMES pango PATH_SUFFIXES pango pango-1.0 REQUIRED)

	find_library(LIB_PNG png REQUIRED)
	
	add_library(CruPlatformGraphicsCairo SHARED
	    Base.cpp
		CairoBrush.cpp
		CairoGeometry.cpp
		CairoGraphicsFactory.cpp
		CairoImage.cpp
		CairoImageFactory.cpp
		CairoPainter.cpp
		CairoResource.cpp
		PangoFont.cpp
		PangoTextLayout.cpp
	)
	target_compile_definitions(CruPlatformGraphicsCairo PRIVATE CRU_PLATFORM_GRAPHICS_CAIRO_EXPORT_API)
	target_link_libraries(CruPlatformGraphicsCairo PUBLIC CruPlatformGraphics PUBLIC ${LIB_GOBJECT} ${LIB_CAIRO} ${LIB_PANGO} ${LIB_PANGOCAIRO} ${LIB_PNG})
	target_include_directories(CruPlatformGraphicsCairo PUBLIC ${CAIRO_HEADER_DIR} ${GLIB_HEADER_DIR} ${GLIBCONFIG_HEADER_DIR} ${HARFBUZZ_HEADER_DIR} ${PANGO_HEADER_DIR})
endif()
