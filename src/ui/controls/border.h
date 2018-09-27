#pragma once

#include <initializer_list>

#include "ui/control.h"

namespace cru::ui::controls
{
    class Border : public Control
    {
    public:
        static Border* Create(const std::initializer_list<Control*>& children = std::initializer_list<Control*>())
        {
            const auto border = new Border();
            for (const auto control : children)
                border->AddChild(control);
            return border;
        }

    protected:
        Border();

    public:
        Border(const Border& other) = delete;
        Border(Border&& other) = delete;
        Border& operator=(const Border& other) = delete;
        Border& operator=(Border&& other) = delete;
        ~Border() override = default;

        bool IsDrawBorder() const
        {
            return draw_border_;
        }

        void SetDrawBorder(bool draw_border);

        Microsoft::WRL::ComPtr<ID2D1Brush> GetBorderBrush() const
        {
            return border_brush_;
        }

        void SetBorderBrush(Microsoft::WRL::ComPtr<ID2D1Brush> border_brush);

        float GetBorderWidth() const
        {
            return border_width_;
        }

        void SetBorderWidth(float border_width);

        Microsoft::WRL::ComPtr<ID2D1StrokeStyle> GetBorderStrokeStyle() const
        {
            return border_stroke_style_;
        }

        void SetBorderStrokeStyle(Microsoft::WRL::ComPtr<ID2D1StrokeStyle> stroke_style);

        float GetBorderRadiusX() const
        {
            return border_radius_x_;
        }

        void SetBorderRadiusX(float border_radius_x);

        float GetBorderRadiusY() const
        {
            return border_radius_y_;
        }

        void SetBorderRadiusY(float border_radius_y);

    protected:
        void OnDraw(ID2D1DeviceContext* device_context) override;

    private:
        bool draw_border_ = true;

        Microsoft::WRL::ComPtr<ID2D1Brush> border_brush_;
        float border_width_ = 1.0f;
        Microsoft::WRL::ComPtr<ID2D1StrokeStyle> border_stroke_style_ = nullptr;

        float border_radius_x_ = 0.0f;
        float border_radius_y_ = 0.0f;
    };
}
