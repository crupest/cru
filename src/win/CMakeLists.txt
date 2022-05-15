add_library(CruWinBase SHARED
	ForDllExport.cpp
)
target_compile_definitions(CruWinBase PUBLIC UNICODE _UNICODE) # use unicode
target_compile_definitions(CruWinBase PRIVATE CRU_WIN_EXPORT_API)
target_link_libraries(CruWinBase PUBLIC CruBase)

add_subdirectory(graphics)
add_subdirectory(gui)
