#include "cru/parse/TokenType.hpp"

namespace cru::parse {
TokenType::TokenType(String name) : name_(std::move(name)) {}

TokenType::~TokenType() {}

}  // namespace cru::parse
