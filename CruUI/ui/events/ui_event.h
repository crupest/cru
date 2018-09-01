#pragma once

#include "system_headers.h"
#include <optional>

#include "base.h"
#include "cru_event.h"
#include "ui/ui_base.h"

namespace cru
{
    namespace ui
    {
        class Control;

        namespace events
        {
            class UiEventArgs : public BasicEventArgs
            {
            public:
                UiEventArgs(Object* sender, Object* original_sender)
                    : BasicEventArgs(sender), original_sender_(original_sender)
                {

                }

                UiEventArgs(const UiEventArgs& other) = default;
                UiEventArgs(UiEventArgs&& other) = default;
                UiEventArgs& operator=(const UiEventArgs& other) = default;
                UiEventArgs& operator=(UiEventArgs&& other) = default;
                ~UiEventArgs() override = default;

                Object* GetOriginalSender() const
                {
                    return original_sender_;
                }

            private:
                Object* original_sender_;
            };


            class MouseEventArgs : public UiEventArgs
            {
            public:
                MouseEventArgs(Object* sender, Object* original_sender, const std::optional<Point>& point = std::nullopt)
                    : UiEventArgs(sender, original_sender), point_(point)
                {
                    
                }
                MouseEventArgs(const MouseEventArgs& other) = default;
                MouseEventArgs(MouseEventArgs&& other) = default;
                MouseEventArgs& operator=(const MouseEventArgs& other) = default;
                MouseEventArgs& operator=(MouseEventArgs&& other) = default;
                ~MouseEventArgs() override = default;

                Point GetPoint(Control* control) const;

            private:
                std::optional<Point> point_;
            };


            class MouseButtonEventArgs : public MouseEventArgs
            {
            public:
                MouseButtonEventArgs(Object* sender, Object* original_sender, const Point& point, const MouseButton button)
                    : MouseEventArgs(sender, original_sender, point), button_(button)
                {
                    
                }
                MouseButtonEventArgs(const MouseButtonEventArgs& other) = default;
                MouseButtonEventArgs(MouseButtonEventArgs&& other) = default;
                MouseButtonEventArgs& operator=(const MouseButtonEventArgs& other) = default;
                MouseButtonEventArgs& operator=(MouseButtonEventArgs&& other) = default;
                ~MouseButtonEventArgs() override = default;

                MouseButton GetMouseButton() const
                {
                    return button_;
                }

            private:
                MouseButton button_;
            };


            class DrawEventArgs : public UiEventArgs
            {
            public:
                DrawEventArgs(Object* sender, Object* original_sender, ID2D1DeviceContext* device_context)
                    : UiEventArgs(sender, original_sender), device_context_(device_context)
                {
                    
                }
                DrawEventArgs(const DrawEventArgs& other) = default;
                DrawEventArgs(DrawEventArgs&& other) = default;
                DrawEventArgs& operator=(const DrawEventArgs& other) = default;
                DrawEventArgs& operator=(DrawEventArgs&& other) = default;
                ~DrawEventArgs() = default;

                ID2D1DeviceContext* GetDeviceContext() const
                {
                    return device_context_;
                }

            private:
                ID2D1DeviceContext * device_context_;
            };


            class PositionChangedEventArgs : public UiEventArgs
            {
            public:
                PositionChangedEventArgs(Object* sender, Object* original_sender, const Point& old_position, const Point& new_position)
                    : UiEventArgs(sender, original_sender), old_position_(old_position), new_position_(new_position)
                {
                    
                }
                PositionChangedEventArgs(const PositionChangedEventArgs& other) = default;
                PositionChangedEventArgs(PositionChangedEventArgs&& other) = default;
                PositionChangedEventArgs& operator=(const PositionChangedEventArgs& other) = default;
                PositionChangedEventArgs& operator=(PositionChangedEventArgs&& other) = default;
                ~PositionChangedEventArgs() override = default;

                Point GetOldPosition() const
                {
                    return old_position_;
                }

                Point GetNewPosition() const
                {
                    return new_position_;
                }

            private:
                Point old_position_;
                Point new_position_;
            };


            class SizeChangedEventArgs : public UiEventArgs
            {
            public:
                SizeChangedEventArgs(Object* sender, Object* original_sender, const Size& old_size, const Size& new_size)
                    : UiEventArgs(sender, original_sender), old_size_(old_size), new_size_(new_size)
                {
                    
                }
                SizeChangedEventArgs(const SizeChangedEventArgs& other) = default;
                SizeChangedEventArgs(SizeChangedEventArgs&& other) = default;
                SizeChangedEventArgs& operator=(const SizeChangedEventArgs& other) = default;
                SizeChangedEventArgs& operator=(SizeChangedEventArgs&& other) = default;
                ~SizeChangedEventArgs() override = default;

                Size GetOldSize() const
                {
                    return old_size_;
                }

                Size GetNewSize() const
                {
                    return new_size_;
                }

            private:
                Size old_size_;
                Size new_size_;
            };


            using UiEvent = Event<UiEventArgs>;
            using MouseEvent = Event<MouseEventArgs>;
            using MouseButtonEvent = Event<MouseButtonEventArgs>;
            using DrawEvent = Event<DrawEventArgs>;
            using PositionChangedEvent = Event<PositionChangedEventArgs>;
            using SizeChangedEvent = Event<SizeChangedEventArgs>;
        }
    }
}
