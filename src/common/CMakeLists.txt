add_library(cru_base SHARED
	Base.cpp
	Exception.cpp
	Format.cpp
	Logger.cpp
	PropertyTree.cpp
	String.cpp
	StringUtil.cpp
	io/Stream.cpp
	io/MemoryStream.cpp
)
target_compile_definitions(cru_base PRIVATE CRU_BASE_EXPORT_API)
target_include_directories(cru_base PUBLIC ${CRU_INCLUDE_DIR})
target_compile_definitions(cru_base PUBLIC $<$<CONFIG:Debug>:CRU_DEBUG>)

if (UNIX)
	target_sources(cru_base PRIVATE
		ErrnoException.cpp
		io/UnixFileStream.cpp
	)
endif()

if (WIN32)
	target_compile_definitions(cru_base PUBLIC CRU_PLATFORM_WINDOWS)
elseif(APPLE)
	target_compile_definitions(cru_base PUBLIC CRU_PLATFORM_OSX)
else()
	target_compile_definitions(cru_base PUBLIC CRU_PLATFORM_LINUX)
endif()

find_package(double-conversion CONFIG REQUIRED)
find_package(Microsoft.GSL CONFIG REQUIRED)
target_link_libraries(cru_base PUBLIC Microsoft.GSL::GSL double-conversion::double-conversion)
