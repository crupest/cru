add_library(CruPlatformBase
	Base.cpp
	Color.cpp
)
target_link_libraries(CruPlatformBase PUBLIC CruBase)
target_compile_definitions(CruPlatformBase PRIVATE CRU_PLATFORM_EXPORT_API)

add_subdirectory(graphics)
add_subdirectory(gui)

if (WIN32)
	add_subdirectory(graphics/direct2d)
	if (CRU_BUILD_CAIRO_ON_WINDOWS)
		target_compile_definitions(CruPlatformBase PUBLIC CRU_BUILD_CAIRO_ON_WINDOWS=1)
		add_subdirectory(graphics/cairo)
	endif()
	add_subdirectory(gui/win)
elseif (APPLE)
	add_subdirectory(graphics/quartz)
	# TODO: Re-enable this!
	# add_subdirectory(graphics/cairo)
	add_subdirectory(gui/osx)
elseif (EMSCRIPTEN)
	add_subdirectory(web)
	# add_subdirectory(graphics/cairo)
	add_subdirectory(graphics/web_canvas)
elseif (UNIX)
	add_subdirectory(graphics/cairo)
	add_subdirectory(gui/xcb)
	add_subdirectory(gui/sdl)
endif()

add_subdirectory(bootstrap)
