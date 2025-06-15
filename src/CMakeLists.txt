add_library(CruBase
	Base.cpp
	Buffer.cpp
	Exception.cpp
	Format.cpp
	PropertyTree.cpp
	String.cpp
	StringToNumberConverter.cpp
	StringUtil.cpp
	SubProcess.cpp
	AutoReadStream.cpp
	BufferStream.cpp
	CFileStream.cpp
	Stream.cpp
	ProxyStream.cpp
	Resource.cpp
	MemoryStream.cpp
	Logger.cpp
	StdioLogTarget.cpp
)
target_link_libraries(CruBase PUBLIC double-conversion)
target_compile_definitions(CruBase PRIVATE CRU_BASE_EXPORT_API)
target_include_directories(CruBase PUBLIC ${CRU_INCLUDE_DIR})
target_compile_definitions(CruBase PUBLIC $<$<CONFIG:Debug>:CRU_DEBUG>)

add_subdirectory(gui)
add_subdirectory(graphics)

if (UNIX AND NOT EMSCRIPTEN)
	target_link_libraries(CruBase PRIVATE pthread)
	target_sources(CruBase PRIVATE Unix.cpp)
endif()

if (APPLE)
	find_library(CORE_FOUNDATION CoreFoundation REQUIRED)
	target_sources(CruBase PRIVATE Osx.cpp Unix.cpp)
	target_link_libraries(CruBase PRIVATE ${CORE_FOUNDATION})
endif()

if (WIN32)
	target_link_libraries(CruBase PUBLIC Shlwapi.lib)
	target_sources(CruBase PRIVATE Windows.cpp)
	target_compile_definitions(CruBase PUBLIC
		CRU_PLATFORM_WINDOWS
		_CRT_SECURE_NO_WARNINGS
	)
endif()

if (MSVC)
	target_compile_options(CruBase PUBLIC /wd4250 /wd4251)
endif()

if (EMSCRIPTEN)
	target_compile_options(CruBase PUBLIC "-fwasm-exceptions")
	target_link_options(CruBase PUBLIC "-fwasm-exceptions")
	target_sources(CruBase PRIVATE platform/web/WebException.cpp)
endif()

add_subdirectory(ui)
add_subdirectory(parse)
add_subdirectory(toml)
add_subdirectory(xml)

add_subdirectory(ThemeBuilder)
