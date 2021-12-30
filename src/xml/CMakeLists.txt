add_library(cru_xml SHARED
	XmlNode.cpp
	XmlParser.cpp
)
target_link_libraries(cru_xml PUBLIC cru_base)
