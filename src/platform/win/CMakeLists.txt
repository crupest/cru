add_library(CruWinBase
	ForDllExport.cpp
)
target_compile_definitions(CruWinBase PUBLIC UNICODE _UNICODE) # use unicode
target_compile_definitions(CruWinBase PRIVATE CRU_WIN_EXPORT_API)
target_link_libraries(CruWinBase PUBLIC CruBase)
