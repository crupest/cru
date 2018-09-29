#pragma once

#include "ui/control.h"

namespace cru::ui::controls
{
    class BorderProperty : public PropertyChangedNotifyObject
    {
    public:
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

    class BorderControl : public Control
    {
        
    };
}
