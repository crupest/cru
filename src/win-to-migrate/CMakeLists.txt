add_library(CruWinBase
	ForDllExport.cpp
	BridgeComStream.cpp
	ComAutoInit.cpp
	DebugLogTarget.cpp
	Exception.cpp
	StreamConvert.cpp
	Win32FileStream.cpp
)
target_compile_definitions(CruWinBase PUBLIC UNICODE _UNICODE) # use unicode
target_compile_definitions(CruWinBase PRIVATE CRU_WIN_EXPORT_API)
target_link_libraries(CruWinBase PUBLIC CruBase)
target_link_libraries(CruBase PUBLIC Shlwapi.lib)
