set(CRU_BASE_INCLUDE_DIR ${CRU_INCLUDE_DIR}/cru/common)
add_library(cru_base STATIC 
	Logger.cpp
)
target_sources(cru_base PUBLIC
	${CRU_BASE_INCLUDE_DIR}/Base.hpp
	${CRU_BASE_INCLUDE_DIR}/Bitmask.hpp
	${CRU_BASE_INCLUDE_DIR}/Event.hpp
	${CRU_BASE_INCLUDE_DIR}/Logger.hpp
	${CRU_BASE_INCLUDE_DIR}/PreConfig.hpp
	${CRU_BASE_INCLUDE_DIR}/SelfResolvable.hpp
)
target_include_directories(cru_base PUBLIC ${CRU_INCLUDE_DIR})
target_compile_definitions(cru_base PUBLIC $<$<CONFIG:Debug>:CRU_DEBUG>)

find_package(Microsoft.GSL CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
target_link_libraries(cru_base PUBLIC Microsoft.GSL::GSL fmt::fmt)
