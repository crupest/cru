#include "cru/ui/mapper/Mapper.hpp"

#include <typeindex>

namespace cru::ui::mapper {
MapperBase::MapperBase(std::type_index type_index)
    : type_index_(std::move(type_index)) {}
}  // namespace cru::ui::mapper
