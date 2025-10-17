#include "cru/parse/TokenType.h"

namespace cru::parse {
TokenType::TokenType(std::string name) : name_(std::move(name)) {}

TokenType::~TokenType() {}

}  // namespace cru::parse
