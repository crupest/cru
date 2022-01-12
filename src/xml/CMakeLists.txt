add_library(cru_xml SHARED
	XmlNode.cpp
	XmlParser.cpp
)
target_compile_definitions(cru_xml PRIVATE CRU_XML_EXPORT_API)
target_link_libraries(cru_xml PUBLIC cru_base)
