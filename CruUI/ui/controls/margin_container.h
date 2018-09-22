#pragma once

#include <initializer_list>

#include "ui/control.h"

namespace cru::ui::controls
{
    class MarginContainer : public Control
    {
    public:
        static MarginContainer* Create(const Thickness& margin = Thickness::Zero(), const std::initializer_list<Control*>& children = std::initializer_list<Control*>())
        {
            const auto margin_container = new MarginContainer(margin);
            for (const auto control : children)
                margin_container->AddChild(control);
            return margin_container;
        }

    protected:
        explicit MarginContainer(const Thickness& margin);

    public:
        MarginContainer(const MarginContainer& other) = delete;
        MarginContainer(MarginContainer&& other) = delete;
        MarginContainer& operator=(const MarginContainer& other) = delete;
        MarginContainer& operator=(MarginContainer&& other) = delete;
        ~MarginContainer() override = default;

        Thickness GetMargin() const
        {
            return margin_;
        }

        void SetMargin(const Thickness& margin);

    protected:
        Size OnMeasure(const Size& available_size) override;
        void OnLayout(const Rect& rect) override;

    private:
        Thickness margin_;
    };
}
