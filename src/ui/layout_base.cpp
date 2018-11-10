#include "layout_base.hpp"

#include "application.hpp"
#include "control.hpp"
#include "window.hpp"

namespace cru::ui
{
    LayoutManager* LayoutManager::GetInstance()
    {
        return Application::GetInstance()->ResolveSingleton<LayoutManager>([](auto)
        {
            return new LayoutManager{};
        });
    }

    void LayoutManager::InvalidateControlPositionCache(Control * control)
    {
        if (cache_invalid_controls_.count(control) == 1)
            return;

        // find descendant then erase it; find ancestor then just return.
        auto i = cache_invalid_controls_.cbegin();
        while (i != cache_invalid_controls_.cend())
        {
            auto current_i = i++;
            const auto result = IsAncestorOrDescendant(*current_i, control);
            if (result == control)
                cache_invalid_controls_.erase(current_i);
            else if (result != nullptr)
                return; // find a ancestor of "control", just return
        }

        cache_invalid_controls_.insert(control);

        if (cache_invalid_controls_.size() == 1) // when insert just now and not repeat to "InvokeLater".
        {
            InvokeLater([this] {
                for (const auto i : cache_invalid_controls_)
                    RefreshControlPositionCache(i);
                for (const auto i : cache_invalid_controls_) // traverse all descendants of position-invalid controls and notify position change event
                    i->TraverseDescendants([](Control* control)
                {
                    control->CheckAndNotifyPositionChanged();
                });
                cache_invalid_controls_.clear(); // after update and notify, clear the set.

            });
        }
    }

    void LayoutManager::RefreshInvalidControlPositionCache()
    {
        for (const auto i : cache_invalid_controls_)
            RefreshControlPositionCache(i);
        cache_invalid_controls_.clear();
    }

    void LayoutManager::RefreshControlPositionCache(Control * control)
    {
        auto point = Point::Zero();
        auto parent = control;
        while ((parent = parent->GetParent())) {
            const auto p = parent->GetPositionRelative();
            point.x += p.x;
            point.y += p.y;
        }
        RefreshControlPositionCacheInternal(control, point);
    }

    void LayoutManager::RefreshControlPositionCacheInternal(Control * control, const Point & parent_lefttop_absolute)
    {
        const auto position = control->GetPositionRelative();
        const Point lefttop(
            parent_lefttop_absolute.x + position.x,
            parent_lefttop_absolute.y + position.y
        );
        control->position_cache_.lefttop_position_absolute = lefttop;
        for(auto c : control->GetChildren())
        {
            RefreshControlPositionCacheInternal(c, lefttop);
        }
    }
}
