add_library(cru_parse SHARED
	Grammar.cpp
	Nonterminal.cpp
	Production.cpp
	Symbol.cpp
	Terminal.cpp
	Token.cpp
	TokenType.cpp
)
target_link_libraries(cru_parse PUBLIC cru_base)
