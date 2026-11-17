#pragma once

#include "../Base.h"

#include <cru/base/ClonePtr.h>
#include <cru/base/Xml.h>
#include <cru/base/datamodel/DataType.h>
#include <cru/base/datamodel/DataTypeRegistry.h>
#include <cru/base/datamodel/DataValidator.h>

#include <memory>

namespace cru::ui::datamodel {
using namespace cru::datamodel;

template <typename T>
using SharedPtrDataTypeBase = DataTypeBase<std::shared_ptr<T>>;

template <typename T>
using ClonePtrDataTypeBase = DataTypeBase<ClonePtr<T>>;

CRU_UI_API DataTypeRegistry* GetUiDataTypeRegistry();
}  // namespace cru::ui::datamodel
