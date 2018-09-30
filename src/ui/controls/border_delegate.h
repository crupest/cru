#pragma once

#include "ui/control.h"

namespace cru::ui::controls
{
    class BorderProperty : public PropertyChangedNotifyObject
    {
    public:
        constexpr static auto brush_property_name = L"Brush";
        constexpr static auto width_property_name = L"Width";
        constexpr static auto stroke_style_property_name = L"StrokeStyle";
        constexpr static auto radius_x_property_name = L"RadiusX";
        constexpr static auto radius_y_property_name = L"RadiusY";

        BorderProperty() = default;
        BorderProperty(const BorderProperty& other) = delete;
        BorderProperty(BorderProperty&& other) = delete;
        BorderProperty& operator=(const BorderProperty& other) = delete;
        BorderProperty& operator=(BorderProperty&& other) = delete;
        ~BorderProperty() override = default;


        Microsoft::WRL::ComPtr<ID2D1Brush> GetBrush() const
        {
            return brush_;
        }

        float GetWidth() const
        {
            return width_;
        }

        Microsoft::WRL::ComPtr<ID2D1StrokeStyle> GetStrokeStyle() const
        {
            return stroke_style_;
        }

        float GetRadiusX() const
        {
            return radius_x_;
        }

        float GetRadiusY() const
        {
            return radius_y_;
        }

        void SetBrush(Microsoft::WRL::ComPtr<ID2D1Brush> brush);
        void SetWidth(float width);
        void SetStrokeStyle(Microsoft::WRL::ComPtr<ID2D1StrokeStyle> stroke_style);
        void SetRadiusX(float radius_x);
        void SetRadiusY(float radius_y);

    private:
        Microsoft::WRL::ComPtr<ID2D1Brush> brush_ = nullptr;
        float width_ = 1.0f;
        Microsoft::WRL::ComPtr<ID2D1StrokeStyle> stroke_style_ = nullptr;
        float radius_x_ = 0.0f;
        float radius_y_ = 0.0f;
    };

    class BorderDelegate : public Object
    {
    public:
        explicit BorderDelegate(Control* control);
        BorderDelegate(Control* control, std::shared_ptr<BorderProperty> border_property);
        BorderDelegate(const BorderDelegate& other) = delete;
        BorderDelegate(BorderDelegate&& other) = delete;
        BorderDelegate& operator=(const BorderDelegate& other) = delete;
        BorderDelegate& operator=(BorderDelegate&& other) = delete;
        ~BorderDelegate() override = default;

        std::shared_ptr<BorderProperty> GetBorderProperty() const
        {
            return border_property_;
        }

        void SetBorderProperty(std::shared_ptr<BorderProperty> border_property);

        void Draw(ID2D1DeviceContext* device_context, const Size& size) const;

        Size GetBorderSize() const;
        Rect CoerceLayoutRect(const Rect& rect) const;

    private:
        Control* control_;
        std::shared_ptr<BorderProperty> border_property_;
        FunctionPtr<void(String)> border_property_changed_listener_;
    };
}
