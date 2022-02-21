add_library(cru_base SHARED
	Base.cpp
	Exception.cpp
	Format.cpp
	PropertyTree.cpp
	String.cpp
	StringUtil.cpp
	io/Stream.cpp
	io/Resource.cpp
	io/MemoryStream.cpp
	log/Logger.cpp
	log/StdioLogTarget.cpp
	platform/Exception.cpp
)
target_compile_definitions(cru_base PRIVATE CRU_BASE_EXPORT_API)
target_include_directories(cru_base PUBLIC ${CRU_INCLUDE_DIR})
target_compile_definitions(cru_base PUBLIC $<$<CONFIG:Debug>:CRU_DEBUG>)

if (UNIX)
	target_sources(cru_base PRIVATE
		platform/unix/ErrnoException.cpp
		platform/unix/UnixFileStream.cpp
	)
endif()

if (APPLE)
	find_library(CORE_FOUNDATION CoreFoundation REQUIRED)
	target_link_libraries(cru_base PUBLIC ${CORE_FOUNDATION})

	target_sources(cru_base PRIVATE
		platform/osx/Convert.cpp
		platform/osx/Exception.cpp
	)
endif()

if (WIN32)
	target_sources(cru_base PRIVATE
		platform/win/BridgeComStream.cpp
		platform/win/ComAutoInit.cpp
		platform/win/DebugLogTarget.cpp
		platform/win/Exception.cpp
		platform/win/StreamConvert.cpp
		platform/win/Win32FileStream.cpp
	)

	target_link_libraries(cru_base PUBLIC Shlwapi.lib)
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
