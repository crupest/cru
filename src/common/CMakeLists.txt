add_library(CruBase
	Base.cpp
	Exception.cpp
	Format.cpp
	PropertyTree.cpp
	String.cpp
	StringToNumberConverter.cpp
	StringUtil.cpp
	io/CFileStream.cpp
	io/Stream.cpp
	io/Resource.cpp
	io/MemoryStream.cpp
	log/Logger.cpp
	log/StdioLogTarget.cpp
	platform/Exception.cpp
)
target_compile_definitions(CruBase PRIVATE CRU_BASE_EXPORT_API)
target_include_directories(CruBase PUBLIC ${CRU_INCLUDE_DIR})
target_compile_definitions(CruBase PUBLIC $<$<CONFIG:Debug>:CRU_DEBUG>)

if (UNIX AND NOT EMSCRIPTEN)
	target_sources(CruBase PRIVATE
		platform/unix/UnixFileStream.cpp
	)

	if (NOT APPLE)
		target_link_libraries(CruBase PUBLIC pthread)
	endif()
endif()

if (APPLE)
	find_library(CORE_FOUNDATION CoreFoundation REQUIRED)
	target_link_libraries(CruBase PUBLIC ${CORE_FOUNDATION})

	target_sources(CruBase PRIVATE
		platform/osx/Convert.cpp
		platform/osx/Exception.cpp
	)
endif()

if (EMSCRIPTEN)
	target_compile_options(CruBase PUBLIC "-fwasm-exceptions")
	target_link_options(CruBase PUBLIC "-fwasm-exceptions")

	target_sources(CruBase PRIVATE
		platform/web/WebException.cpp
	)
endif()

if (WIN32)
	target_sources(CruBase PRIVATE
		platform/win/BridgeComStream.cpp
		platform/win/ComAutoInit.cpp
		platform/win/DebugLogTarget.cpp
		platform/win/Exception.cpp
		platform/win/StreamConvert.cpp
		platform/win/Win32FileStream.cpp
	)

	target_link_libraries(CruBase PUBLIC Shlwapi.lib)
endif()

if (WIN32)
	target_compile_definitions(CruBase PUBLIC CRU_PLATFORM_WINDOWS)
elseif(APPLE)
	target_compile_definitions(CruBase PUBLIC CRU_PLATFORM_OSX)
elseif(EMSCRIPTEN)
	target_compile_definitions(CruBase PUBLIC CRU_PLATFORM_EMSCRIPTEN)
else()
	target_compile_definitions(CruBase PUBLIC CRU_PLATFORM_LINUX)
endif()

target_link_libraries(CruBase PUBLIC GSL double-conversion)
