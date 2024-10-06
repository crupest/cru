#pragma once
#include "../Base.h"
#include "cru/base/Base.h"
#include "cru/base/Event.h"

namespace cru::ui::model {
enum ListChangeType {
  kItemAdd,
  kItemRemove,
  kItemSet,
  kItemMove,
  kClear,
};

struct CRU_UI_API ListChange {
  ListChangeType type;
  union {
    Index position;
  };
  union {
    Index count;
    Index new_position;
  };

  constexpr static ListChange ItemAdd(Index index, Index count = 1) {
    return {ListChangeType::kItemAdd, index, count};
  }

  constexpr static ListChange ItemRemove(Index index, Index count = 1) {
    return {ListChangeType::kItemRemove, index, count};
  }

  constexpr static ListChange ItemSet(Index index, Index count = 1) {
    return {ListChangeType::kItemSet, index, count};
  }

  constexpr static ListChange ItemMove(Index position, Index new_position) {
    return {ListChangeType::kItemMove, position, new_position};
  }

  constexpr static ListChange Clear() { return {ListChangeType::kClear}; }
};

struct CRU_UI_API IListChangeNotify : virtual Interface {
  virtual IEvent<ListChange>* ListChangeEvent() = 0;
};
}  // namespace cru::ui::model
