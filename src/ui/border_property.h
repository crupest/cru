#pragma once

#include "system_headers.h"

#include "base.h"


namespace cru::ui
{
    class BorderProperty final : public PropertyChangedNotifyObject
    {
    public:
        constexpr static auto brush_property_name = L"Brush";
        constexpr static auto width_property_name = L"StrokeWidth";
        constexpr static auto stroke_style_property_name = L"StrokeStyle";
        constexpr static auto radius_x_property_name = L"RadiusX";
        constexpr static auto radius_y_property_name = L"RadiusY";

        using Ptr = std::shared_ptr<BorderProperty>;

        static Ptr Create()
        {
            return std::make_shared<BorderProperty>();
        }

        BorderProperty();
        BorderProperty(const BorderProperty& other) = delete;
        BorderProperty(BorderProperty&& other) = delete;
        BorderProperty& operator=(const BorderProperty& other) = delete;
        BorderProperty& operator=(BorderProperty&& other) = delete;
        ~BorderProperty() override = default;


        Microsoft::WRL::ComPtr<ID2D1Brush> GetBrush() const
        {
            return brush_;
        }

        float GetStrokeWidth() const
        {
            return stroke_width_;
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
        float stroke_width_ = 1.0f;
        Microsoft::WRL::ComPtr<ID2D1StrokeStyle> stroke_style_ = nullptr;
        float radius_x_ = 0.0f;
        float radius_y_ = 0.0f;
    };
}
