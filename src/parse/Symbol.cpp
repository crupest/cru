#include "cru/parse/Symbol.hpp"

namespace cru::parse {
Symbol::Symbol(Grammar* grammar, String name)
    : grammar_(grammar), name_(std::move(name)) {}

Symbol::~Symbol() {}
}  // namespace cru::parse
