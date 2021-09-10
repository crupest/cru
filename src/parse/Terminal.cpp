#include "cru/parse/Terminal.hpp"

namespace cru::parse {
Terminal::Terminal(Grammar* grammar, String name) : Symbol(grammar, name) {}

Terminal::~Terminal() {}
}  // namespace cru::parse
