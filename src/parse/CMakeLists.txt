add_library(CruParse SHARED
	Grammar.cpp
	Nonterminal.cpp
	ParsingAlgorithm.cpp
	ParsingAlgorithmContext.cpp
	ParsingContext.cpp
	ParsingTreeNode.cpp
	Production.cpp
	RecursiveDescentAlgorithm.cpp
	RecursiveDescentAlgorithmContext.cpp
	Symbol.cpp
	Terminal.cpp
	Token.cpp
	TokenType.cpp
)
target_compile_definitions(CruParse PRIVATE CRU_PARSE_EXPORT_API)
target_link_libraries(CruParse PUBLIC CruBase)
