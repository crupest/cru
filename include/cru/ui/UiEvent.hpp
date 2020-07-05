#pragma once
#include "Base.hpp"

#include "cru/common/Event.hpp"
#include "cru/platform/native/Keyboard.hpp"

#include <memory>
#include <optional>
#include <string>
#include <type_traits>

namespace cru::platform::graph {
struct IPainter;
}

namespace cru::ui::event {
class UiEventArgs : public Object {
 public:
  UiEventArgs(Object* sender, Object* original_sender)
      : sender_(sender), original_sender_(original_sender), handled_(false) {}

  UiEventArgs(const UiEventArgs& other) = default;
  UiEventArgs(UiEventArgs&& other) = default;
  UiEventArgs& operator=(const UiEventArgs& other) = default;
  UiEventArgs& operator=(UiEventArgs&& other) = default;
  ~UiEventArgs() override = default;

  Object* GetSender() const { return sender_; }

  Object* GetOriginalSender() const { return original_sender_; }

  bool IsHandled() const { return handled_; }
  void SetHandled(const bool handled = true) { handled_ = handled; }

 private:
  Object* sender_;
  Object* original_sender_;
  bool handled_;
};

// TEventArgs must not be a reference type. This class help add reference.
// EventArgs must be reference because the IsHandled property must be settable.
template <typename TEventArgs>
class RoutedEvent {
 public:
  static_assert(std::is_base_of_v<UiEventArgs, TEventArgs>,
                "TEventArgs must be subclass of UiEventArgs.");
  static_assert(!std::is_reference_v<TEventArgs>,
                "TEventArgs must not be reference.");

  using RawEventArgs = TEventArgs;
  using IEventType = IEvent<TEventArgs&>;
  using EventArgs = typename IEventType::EventArgs;

  RoutedEvent() = default;
  RoutedEvent(const RoutedEvent& other) = delete;
  RoutedEvent(RoutedEvent&& other) = delete;
  RoutedEvent& operator=(const RoutedEvent& other) = delete;
  RoutedEvent& operator=(RoutedEvent&& other) = delete;
  ~RoutedEvent() = default;

  IEvent<TEventArgs&>* Direct() { return &direct_; }

  IEvent<TEventArgs&>* Bubble() { return &bubble_; }

  IEvent<TEventArgs&>* Tunnel() { return &tunnel_; }

 private:
  Event<TEventArgs&> direct_;
  Event<TEventArgs&> bubble_;
  Event<TEventArgs&> tunnel_;
};

class MouseEventArgs : public UiEventArgs {
 public:
  MouseEventArgs(Object* sender, Object* original_sender,
                 const std::optional<Point>& point = std::nullopt)
      : UiEventArgs(sender, original_sender), point_(point) {}
  MouseEventArgs(const MouseEventArgs& other) = default;
  MouseEventArgs(MouseEventArgs&& other) = default;
  MouseEventArgs& operator=(const MouseEventArgs& other) = default;
  MouseEventArgs& operator=(MouseEventArgs&& other) = default;
  ~MouseEventArgs() override = default;

  // This point is relative to window client lefttop.
  Point GetPoint() const { return point_.value_or(Point{}); }
  Point GetPointToContent(render::RenderObject* render_target) const;

 private:
  std::optional<Point> point_;
};

class MouseButtonEventArgs : public MouseEventArgs {
 public:
  MouseButtonEventArgs(Object* sender, Object* original_sender,
                       const Point& point, const MouseButton button,
                       platform::native::KeyModifier key_modifier)
      : MouseEventArgs(sender, original_sender, point),
        button_(button),
        key_modifier_(key_modifier) {}
  MouseButtonEventArgs(Object* sender, Object* original_sender,
                       const MouseButton button,
                       platform::native::KeyModifier key_modifier)
      : MouseEventArgs(sender, original_sender),
        button_(button),
        key_modifier_(key_modifier) {}
  MouseButtonEventArgs(const MouseButtonEventArgs& other) = default;
  MouseButtonEventArgs(MouseButtonEventArgs&& other) = default;
  MouseButtonEventArgs& operator=(const MouseButtonEventArgs& other) = default;
  MouseButtonEventArgs& operator=(MouseButtonEventArgs&& other) = default;
  ~MouseButtonEventArgs() override = default;

  MouseButton GetButton() const { return button_; }
  platform::native::KeyModifier GetKeyModifier() const { return key_modifier_; }

 private:
  MouseButton button_;
  platform::native::KeyModifier key_modifier_;
};

class MouseWheelEventArgs : public MouseEventArgs {
 public:
  MouseWheelEventArgs(Object* sender, Object* original_sender,
                      const Point& point, const float delta)
      : MouseEventArgs(sender, original_sender, point), delta_(delta) {}
  MouseWheelEventArgs(const MouseWheelEventArgs& other) = default;
  MouseWheelEventArgs(MouseWheelEventArgs&& other) = default;
  MouseWheelEventArgs& operator=(const MouseWheelEventArgs& other) = default;
  MouseWheelEventArgs& operator=(MouseWheelEventArgs&& other) = default;
  ~MouseWheelEventArgs() override = default;

  float GetDelta() const { return delta_; }

 private:
  float delta_;
};

class PaintEventArgs : public UiEventArgs {
 public:
  PaintEventArgs(Object* sender, Object* original_sender,
                 platform::graph::IPainter* painter)
      : UiEventArgs(sender, original_sender), painter_(painter) {}
  PaintEventArgs(const PaintEventArgs& other) = default;
  PaintEventArgs(PaintEventArgs&& other) = default;
  PaintEventArgs& operator=(const PaintEventArgs& other) = default;
  PaintEventArgs& operator=(PaintEventArgs&& other) = default;
  ~PaintEventArgs() = default;

  platform::graph::IPainter* GetPainter() const { return painter_; }

 private:
  platform::graph::IPainter* painter_;
};

class FocusChangeEventArgs : public UiEventArgs {
 public:
  FocusChangeEventArgs(Object* sender, Object* original_sender,
                       const bool is_window = false)
      : UiEventArgs(sender, original_sender), is_window_(is_window) {}
  FocusChangeEventArgs(const FocusChangeEventArgs& other) = default;
  FocusChangeEventArgs(FocusChangeEventArgs&& other) = default;
  FocusChangeEventArgs& operator=(const FocusChangeEventArgs& other) = default;
  FocusChangeEventArgs& operator=(FocusChangeEventArgs&& other) = default;
  ~FocusChangeEventArgs() override = default;

  // Return whether the focus change is caused by the window-wide focus change.
  bool IsWindow() const { return is_window_; }

 private:
  bool is_window_;
};

/*
class ToggleEventArgs : public UiEventArgs {
 public:
  ToggleEventArgs(Object* sender, Object* original_sender, bool new_state)
      : UiEventArgs(sender, original_sender), new_state_(new_state) {}
  ToggleEventArgs(const ToggleEventArgs& other) = default;
  ToggleEventArgs(ToggleEventArgs&& other) = default;
  ToggleEventArgs& operator=(const ToggleEventArgs& other) = default;
  ToggleEventArgs& operator=(ToggleEventArgs&& other) = default;
  ~ToggleEventArgs() override = default;

  bool GetNewState() const { return new_state_; }

 private:
  bool new_state_;
};
*/

class KeyEventArgs : public UiEventArgs {
 public:
  KeyEventArgs(Object* sender, Object* original_sender,
               platform::native::KeyCode key_code,
               platform::native::KeyModifier key_modifier)
      : UiEventArgs(sender, original_sender),
        key_code_(key_code),
        key_modifier_(key_modifier) {}
  KeyEventArgs(const KeyEventArgs& other) = default;
  KeyEventArgs(KeyEventArgs&& other) = default;
  KeyEventArgs& operator=(const KeyEventArgs& other) = default;
  KeyEventArgs& operator=(KeyEventArgs&& other) = default;
  ~KeyEventArgs() override = default;

  platform::native::KeyCode GetKeyCode() const { return key_code_; }
  platform::native::KeyModifier GetKeyModifier() const { return key_modifier_; }

 private:
  platform::native::KeyCode key_code_;
  platform::native::KeyModifier key_modifier_;
};

class CharEventArgs : public UiEventArgs {
 public:
  CharEventArgs(Object* sender, Object* original_sender, std::u16string c)
      : UiEventArgs(sender, original_sender), c_(std::move(c)) {}
  CharEventArgs(const CharEventArgs& other) = default;
  CharEventArgs(CharEventArgs&& other) = default;
  CharEventArgs& operator=(const CharEventArgs& other) = default;
  CharEventArgs& operator=(CharEventArgs&& other) = default;
  ~CharEventArgs() override = default;

  std::u16string GetChar() const { return c_; }

 private:
  std::u16string c_;
};
}  // namespace cru::ui::event
