add_library(CruXml SHARED
	XmlNode.cpp
	XmlParser.cpp
)
target_compile_definitions(CruXml PRIVATE CRU_XML_EXPORT_API)
target_link_libraries(CruXml PUBLIC CruBase)
