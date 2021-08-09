set(CRU_BASE_INCLUDE_DIR ${CRU_INCLUDE_DIR}/cru/common)
add_library(cru_base SHARED
	Logger.cpp
	StringUtil.cpp
)
target_sources(cru_base PUBLIC
	${CRU_BASE_INCLUDE_DIR}/Base.hpp
	${CRU_BASE_INCLUDE_DIR}/Bitmask.hpp
	${CRU_BASE_INCLUDE_DIR}/Event.hpp
	${CRU_BASE_INCLUDE_DIR}/Format.hpp
	${CRU_BASE_INCLUDE_DIR}/Logger.hpp
	${CRU_BASE_INCLUDE_DIR}/PreConfig.hpp
	${CRU_BASE_INCLUDE_DIR}/SelfResolvable.hpp
	${CRU_BASE_INCLUDE_DIR}/StringUtil.hpp
)
target_include_directories(cru_base PUBLIC ${CRU_INCLUDE_DIR})
target_compile_definitions(cru_base PUBLIC $<$<CONFIG:Debug>:CRU_DEBUG>)
target_compile_definitions(cru_base PRIVATE CRU_BASE_EXPORT_API)

if (WIN32)
	target_compile_definitions(cru_base PUBLIC CRU_PLATFORM_WINDOWS)
else()
	target_compile_definitions(cru_base PUBLIC CRU_PLATFORM_UNIX)
endif()

find_package(Microsoft.GSL CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
target_link_libraries(cru_base PUBLIC Microsoft.GSL::GSL fmt::fmt)
