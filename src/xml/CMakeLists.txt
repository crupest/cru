add_library(CruXml
	XmlNode.cpp
	XmlParser.cpp
)
target_compile_definitions(CruXml PRIVATE CRU_XML_EXPORT_API)
target_link_libraries(CruXml PUBLIC CruBase)
