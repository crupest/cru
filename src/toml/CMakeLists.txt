add_library(cru_toml SHARED
	TomlDocument.cpp
	TomlParser.cpp
)
target_link_libraries(cru_toml PUBLIC cru_base)
