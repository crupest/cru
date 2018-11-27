#pragma once

// ReSharper disable once CppUnusedIncludeDirective
#include "pre.hpp"

#include "system_headers.hpp"

#include "base.hpp"


namespace cru::ui
{
    class BorderProperty final
    {
    public:
        BorderProperty();
        explicit BorderProperty(Microsoft::WRL::ComPtr<ID2D1Brush> brush);
        BorderProperty(Microsoft::WRL::ComPtr<ID2D1Brush> brush, float width, float radius_x, float radius_y, Microsoft::WRL::ComPtr<ID2D1StrokeStyle> stroke_style = nullptr);
        BorderProperty(const BorderProperty& other) = default;
        BorderProperty(BorderProperty&& other) = default;
        BorderProperty& operator=(const BorderProperty& other) = default;
        BorderProperty& operator=(BorderProperty&& other) = default;
        ~BorderProperty() = default;


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

        void SetBrush(Microsoft::WRL::ComPtr<ID2D1Brush> brush)
        {
            Require(brush == nullptr, "Brush of BorderProperty mustn't be null.");
            brush_ = std::move(brush);
        }

        void SetStrokeWidth(const float stroke_width)
        {
            Require(stroke_width >= 0.0f, "Stroke width must be no less than 0.");
            stroke_width_ = stroke_width;
        }

        void SetStrokeStyle(Microsoft::WRL::ComPtr<ID2D1StrokeStyle> stroke_style)
        {
            stroke_style_ = std::move(stroke_style);
        }

        void SetRadiusX(const float radius_x)
        {
            Require(radius_x >= 0.0f, "Radius-x must be no less than 0.");
            radius_x_ = radius_x;
        }

        void SetRadiusY(const float radius_y)
        {
            Require(radius_y >= 0.0f, "Radius-y must be no less than 0.");
            radius_y_ = radius_y;
        }

    private:
        Microsoft::WRL::ComPtr<ID2D1Brush> brush_;
        float stroke_width_ = 1.0f;
        float radius_x_ = 0.0f;
        float radius_y_ = 0.0f;
        Microsoft::WRL::ComPtr<ID2D1StrokeStyle> stroke_style_ = nullptr;
    };
}
