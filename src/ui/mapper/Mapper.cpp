#include "cru/ui/mapper/Mapper.h"

#include <typeindex>

namespace cru::ui::mapper {
MapperBase::MapperBase(std::type_index type_index)
    : type_index_(std::move(type_index)) {}
}  // namespace cru::ui::mapper
