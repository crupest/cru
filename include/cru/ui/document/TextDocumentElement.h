#pragma once

#include "../Base.h"
#include "DocumentElement.h"
#include "cru/base/Base.h"
#include "cru/base/Bitmask.h"

namespace cru::ui::document {
namespace details {
struct TextStyleTag {};
}  // namespace details

using TextStyle = Bitmask<details::TextStyleTag>;

struct TextStyles {
  static constexpr TextStyle Normal{0x0};
  static constexpr TextStyle Bold{0x1};
  static constexpr TextStyle Italic{0x2};
};

struct IDocumentLink : virtual Interface {
  virtual void Open() = 0;
};

class CRU_UI_API TextDocumentElement : public DocumentElement {
 public:
  TextDocumentElement(String text, TextStyle style, IDocumentLink* link);

  ~TextDocumentElement() override;

  String GetText() const { return text_; }
  void SetText(String text);

  TextStyle GetStyle() const { return style_; }
  void SetStyle(TextStyle style);

  IDocumentLink* GetLink() const { return link_; }
  void SetLink(IDocumentLink link);

 private:
  String text_;
  TextStyle style_;
  IDocumentLink* link_;
};
}  // namespace cru::ui::document
