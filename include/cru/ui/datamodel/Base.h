#pragma once

#include "../Base.h"

#include <cru/base/ClonePtr.h>
#include <cru/base/Xml.h>
#include <cru/base/datamodel/DataType.h>
#include <cru/base/datamodel/DataTypeRegistry.h>
#include <cru/base/datamodel/DataValidator.h>

#include <memory>

namespace cru::ui::datamodel {
using cru::datamodel::DataConvertResult;
using cru::datamodel::DataTypeBase;
using cru::datamodel::DataTypeRegistry;
using cru::datamodel::IDataType;
using cru::datamodel::IDataValidator;
using cru::datamodel::NumberDataType;
using cru::datamodel::NumberRangeDataValidator;
using cru::datamodel::StringDataType;

template <typename T>
using SharedPtrDataTypeBase = DataTypeBase<std::shared_ptr<T>>;

template <typename T>
using ClonePtrDataTypeBase = DataTypeBase<ClonePtr<T>>;

CRU_UI_API cru::datamodel::DataTypeRegistry* GetUiDataTypeRegistry();
}  // namespace cru::ui::datamodel
