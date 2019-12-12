set(CRU_BASE_INCLUDE_DIR ${CRU_INCLUDE_DIR}/cru/common)
add_library(cru_base STATIC 
    logger.cpp
)
target_sources(cru_base PUBLIC
	${CRU_BASE_INCLUDE_DIR}/base.hpp
	${CRU_BASE_INCLUDE_DIR}/event.hpp
	${CRU_BASE_INCLUDE_DIR}/format.hpp
	${CRU_BASE_INCLUDE_DIR}/logger.hpp
	${CRU_BASE_INCLUDE_DIR}/pre_config.hpp
	${CRU_BASE_INCLUDE_DIR}/self_resolvable.hpp
)
target_include_directories(cru_base PUBLIC ${CRU_INCLUDE_DIR})
