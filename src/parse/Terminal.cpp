#include "cru/parse/Terminal.h"

namespace cru::parse {
Terminal::Terminal(Grammar* grammar, String name) : Symbol(grammar, name) {}

Terminal::~Terminal() {}
}  // namespace cru::parse
