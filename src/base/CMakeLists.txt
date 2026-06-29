add_library(CruBase
	Base.cpp
	PropertyTree.cpp
	StringUtil.cpp
	SubProcess.cpp
	io/AutoReadStream.cpp
	io/Base.cpp
	io/BufferStream.cpp
	io/CFileStream.cpp
	io/Stream.cpp
	io/Resource.cpp
	io/MemoryStream.cpp
	log/Logger.cpp
	log/StdioLogWriter.cpp
	toml/TomlDocument.cpp
	toml/TomlParser.cpp
	xml/XmlNode.cpp
	xml/XmlParser.cpp
)
target_compile_definitions(CruBase PRIVATE CRU_BASE_EXPORT_API)
target_include_directories(CruBase PUBLIC ${CRU_INCLUDE_DIR})
target_compile_definitions(CruBase PUBLIC $<$<CONFIG:Debug>:CRU_DEBUG>)

if (APPLE)
	# Homebrew prefix is discovered in the top-level CMakeLists.txt.
	set(_cru_icu_roots
		"${CRU_HOMEBREW_PREFIX}/opt/icu4c"
		"${CRU_HOMEBREW_PREFIX}/opt/icu4c@78"
	)

	foreach(_cru_icu_root IN LISTS _cru_icu_roots)
		if (EXISTS "${_cru_icu_root}/include/unicode/utypes.h" AND EXISTS "${_cru_icu_root}/lib/libicuuc.dylib")
			set(ICU_ROOT "${_cru_icu_root}")
			break()
		endif()
	endforeach()

	if (ICU_ROOT)
		list(PREPEND CMAKE_SYSTEM_PREFIX_PATH "${ICU_ROOT}")
	else()
		message(FATAL_ERROR "Failed to find ICU in Homebrew prefix: ${CRU_HOMEBREW_PREFIX}.")
	endif()
endif()

# Link only the ICU common library. The data library is pulled in automatically
# as a dependency of ICU::uc on most platforms.
# Add ICU::i18n if internationalization APIs (formatting, collation, etc.) are needed.
find_package(ICU MODULE COMPONENTS uc REQUIRED)
target_link_libraries(CruBase PUBLIC ICU::uc)

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
		platform/win/Base.cpp
		platform/win/BridgeComStream.cpp
		platform/win/ComAutoInit.cpp
		platform/win/DebugLogWriter.cpp
		platform/win/Stream.cpp
		platform/win/Win32SubProcess.cpp
	)
	target_compile_definitions(CruBase PUBLIC
		UNICODE _UNICODE
		_CRT_SECURE_NO_WARNINGS
	)
	target_link_libraries(CruBase PUBLIC Shlwapi.lib)
endif()

if (MSVC)
	target_compile_options(CruBase PUBLIC /wd4250 /wd4251)
endif()
