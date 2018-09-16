#pragma once

#include "ui/control.h"

namespace cru::ui::controls
{
    class ToggleButton : public Control
    {
    public:
        static ToggleButton* Create()
        {
            return new ToggleButton();
        }

    protected:
        ToggleButton();

    public:
        ToggleButton(const ToggleButton& other) = delete;
        ToggleButton(ToggleButton&& other) = delete;
        ToggleButton& operator=(const ToggleButton& other) = delete;
        ToggleButton& operator=(ToggleButton&& other) = delete;
        ~ToggleButton() override = default;

        bool IsPointInside(const Point& point) override;

        bool GetState() const
        {
            return state_;
        }

        void SetState(bool state);

        void Toggle();

    public:
        events::ToggleEvent toggle_event;

    protected:
        virtual void OnToggle(events::ToggleEventArgs& args);

    protected:
        void OnDraw(ID2D1DeviceContext* device_context) override;

        void OnMouseClickCore(events::MouseButtonEventArgs& args) override;

        Size OnMeasure(const Size& available_size) override;

    private:
        void OnToggleInternal(bool new_state);

    private:
        bool state_ = false;

        Microsoft::WRL::ComPtr<ID2D1RoundedRectangleGeometry> frame_path_;
        Microsoft::WRL::ComPtr<ID2D1Brush> on_brush_;
        Microsoft::WRL::ComPtr<ID2D1Brush> off_brush_;
    };
}
