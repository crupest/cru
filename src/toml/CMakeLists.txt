add_library(cru_toml SHARED
	TomlDocument.cpp
	TomlParser.cpp
)
target_compile_definitions(cru_toml PRIVATE CRU_TOML_EXPORT_API)
target_link_libraries(cru_toml PUBLIC cru_base)
