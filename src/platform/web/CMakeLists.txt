add_library(CruPlatformBaseWeb
	JsUtility.cpp
	Resource.cpp
)

find_library(EMBIND_LIB embind REQUIRED)

target_link_libraries(CruPlatformBaseWeb PUBLIC CruPlatformBase)
target_link_libraries(CruPlatformBaseWeb PUBLIC EMBIND_LIB)

