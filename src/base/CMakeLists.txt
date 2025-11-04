add_library(CruBase
	Base.cpp
	Buffer.cpp
	PropertyTree.cpp
	StringUtil.cpp
	SubProcess.cpp
	io/AutoReadStream.cpp
	io/BufferStream.cpp
	io/CFileStream.cpp
	io/Stream.cpp
	io/ProxyStream.cpp
	io/Resource.cpp
	io/MemoryStream.cpp
	log/Logger.cpp
	log/StdioLogTarget.cpp
	toml/TomlDocument.cpp
	toml/TomlParser.cpp
	xml/XmlNode.cpp
	xml/XmlParser.cpp
)
target_compile_definitions(CruBase PRIVATE CRU_BASE_EXPORT_API)
target_include_directories(CruBase PUBLIC ${CRU_INCLUDE_DIR})
target_compile_definitions(CruBase PUBLIC $<$<CONFIG:Debug>:CRU_DEBUG>)

if (UNIX AND NOT EMSCRIPTEN)
	target_sources(CruBase PRIVATE
		platform/unix/EventLoop.cpp
		platform/unix/PosixSpawnSubProcess.cpp
		platform/unix/UnixFile.cpp
		platform/unix/UnixFileStream.cpp
	)

	if (NOT APPLE)
		target_link_libraries(CruBase PUBLIC pthread)
	endif()
endif()

if (APPLE)
	target_sources(CruBase PRIVATE platform/osx/Base.cpp)
	find_library(CORE_FOUNDATION CoreFoundation REQUIRED)
	target_link_libraries(CruBase PUBLIC ${CORE_FOUNDATION})
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

if (MSVC)
	target_compile_options(CruBase PUBLIC /wd4250 /wd4251)
endif()

if (WIN32)
	target_compile_definitions(CruBase PUBLIC
		CRU_PLATFORM_WINDOWS
		_CRT_SECURE_NO_WARNINGS
	)
elseif(APPLE)
	target_compile_definitions(CruBase PUBLIC
		CRU_PLATFORM_OSX
		CRU_PLATFORM_UNIX
	)
elseif(EMSCRIPTEN)
	target_compile_definitions(CruBase PUBLIC CRU_PLATFORM_EMSCRIPTEN)
else()
	target_compile_definitions(CruBase PUBLIC
		CRU_PLATFORM_LINUX
		CRU_PLATFORM_UNIX
	)
endif()
