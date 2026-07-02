add_library(CruPlatformGraphicsCairo
	Base.cpp
	CairoBrush.cpp
	CairoGeometry.cpp
	CairoGraphicsFactory.cpp
	CairoImage.cpp
	CairoImageFactory.cpp
	CairoPainter.cpp
	PangoFont.cpp
	PangoTextLayout.cpp
)

find_package(PkgConfig REQUIRED)
find_package(PNG REQUIRED)

pkg_check_modules(PANGO REQUIRED IMPORTED_TARGET pango)
pkg_check_modules(CAIRO REQUIRED IMPORTED_TARGET cairo)
pkg_check_modules(PANGOCAIRO REQUIRED IMPORTED_TARGET pangocairo)

target_compile_definitions(CruPlatformGraphicsCairo PRIVATE CRU_PLATFORM_GRAPHICS_CAIRO_EXPORT_API)
target_link_libraries(CruPlatformGraphicsCairo PUBLIC CruPlatformGraphics PUBLIC
	PNG::PNG
	PkgConfig::PANGO
	PkgConfig::CAIRO
	PkgConfig::PANGOCAIRO
)
