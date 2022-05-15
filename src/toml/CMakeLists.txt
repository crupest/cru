add_library(CruToml SHARED
	TomlDocument.cpp
	TomlParser.cpp
)
target_compile_definitions(CruToml PRIVATE CRU_TOML_EXPORT_API)
target_link_libraries(CruToml PUBLIC CruBase)
