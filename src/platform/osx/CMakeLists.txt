add_library(CruPlatformBaseOsx
	Resource.cpp
)

find_library(FOUNDATION Foundation REQUIRED)
find_library(CORE_FOUNDATION CoreFoundation REQUIRED)

target_link_libraries(CruPlatformBaseOsx PUBLIC CruPlatformBase ${FOUNDATION} ${CORE_FOUNDATION})
