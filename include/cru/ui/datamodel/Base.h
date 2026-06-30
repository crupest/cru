#pragma once

#include "../Base.h"

#include <cru/base/ClonePtr.h>
#include <cru/base/datamodel/DataType.h>
#include <cru/base/datamodel/DataTypeRegistry.h>
#include <cru/base/xml/XmlNode.h>

#include <memory>

namespace cru::ui::datamodel {
using cru::datamodel::DataConvertResult;
using cru::datamodel::DataTypeBase;
using cru::datamodel::DataTypeRegistry;
using cru::datamodel::NumberDataType;
using cru::datamodel::StringDataType;

template <typename T>
using SharedPtrDataTypeBase = DataTypeBase<std::shared_ptr<T>>;

template <typename T>
using ClonePtrDataTypeBase = DataTypeBase<ClonePtr<T>>;

CRU_UI_API cru::datamodel::DataTypeRegistry* GetUiDataTypeRegistry();
}  // namespace cru::ui::datamodel
