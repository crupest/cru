#include "control.hpp"

#include <algorithm>
#include <cassert>

#include "window.hpp"
#include "application.hpp"
#include "graph/graph.hpp"
#include "exception.hpp"
#include "cru_debug.hpp"
#include "d2d_util.hpp"
#include "math_util.hpp"

#ifdef CRU_DEBUG_LAYOUT
#include "ui_manager.hpp"
#endif

namespace cru::ui
{

    void Control::SetParent(Control* parent)
    {
        const auto old_parent = GetParent();
        parent_ = parent;
        const auto new_parent = GetParent();
        if (old_parent != new_parent)
            OnParentChanged(old_parent, new_parent);
    }

    void Control::SetDescendantWindow(Window* window)
    {
        if (window == nullptr && window_ == nullptr)
            return;

        //You can only attach or detach window.
        assert((window != nullptr && window_ == nullptr) || (window == nullptr && window_ != nullptr));

        if (window == nullptr)
        {
            const auto old = window_;
            TraverseDescendants([old](Control* control)
            {
                control->window_ = nullptr;
                control->OnDetachToWindow(old);
            });
        }
        else
            TraverseDescendants([window](Control* control)
            {
                control->window_ = window;
                control->OnAttachToWindow(window);
            });            
    }

    void Control::TraverseDescendants(const std::function<void(Control*)>& predicate)
    {
        TraverseDescendantsInternal(this, predicate);
    }

    void Control::TraverseDescendantsInternal(Control * control, const std::function<void(Control*)>& predicate)
    {
        predicate(control);
        for (auto c: control->GetInternalChildren())
            TraverseDescendantsInternal(c, predicate);
    }


    Point Control::ControlToWindow(const Point& point) const
    {
        const auto position = GetPositionInWindow();
        return Point(point.x + position.x, point.y + position.y);
    }

    Point Control::WindowToControl(const Point & point) const
    {
        const auto position = GetPositionInWindow();
        return Point(point.x - position.x, point.y - position.y);
    }

    bool Control::RequestFocus()
    {
        auto window = GetWindow();
        if (window == nullptr)
            return false;

        return window->RequestFocusFor(this);
    }

    bool Control::HasFocus()
    {
        auto window = GetWindow();
        if (window == nullptr)
            return false;

        return window->GetFocusControl() == this;
    }

    void Control::SetCursor(const Cursor::Ptr& cursor)
    {
        if (cursor != cursor_)
        {
            cursor_ = cursor;
            const auto window = GetWindow();
            if (window && window->GetMouseHoverControl() == this)
                window->UpdateCursor();
        }
    }

    void Control::OnParentChanged(Control* old_parent, Control* new_parent)
    {

    }

    void Control::OnAttachToWindow(Window* window)
    {

    }

    void Control::OnDetachToWindow(Window * window)
    {

    }

    void Control::OnMouseClickBegin(MouseButton button)
    {

    }

    void Control::OnMouseClickEnd(MouseButton button)
    {

    }


    const std::vector<Control*> NoChildControl::empty_control_vector{};


    ContentControl::ContentControl() : child_vector_{nullptr}, child_(child_vector_[0])
    {

    }

    ContentControl::~ContentControl()
    {
        delete child_;
    }

    void ContentControl::SetChild(Control* child)
    {
        if (child == child_)
            return;

        const auto window = GetWindow();
        const auto old_child = child_;
        child_ = child;
        if (old_child)
        {
            old_child->SetParent(nullptr);
            old_child->SetDescendantWindow(nullptr);
        }
        if (child)
        {
            child->SetParent(this);
            child->SetDescendantWindow(window);
        }
        OnChildChanged(old_child, child);
    }

    void ContentControl::OnChildChanged(Control* old_child, Control* new_child)
    {

    }

    void ControlAddChildCheck(Control* control)
    {
        if (control->GetParent() != nullptr)
            throw std::invalid_argument("The control already has a parent.");

        if (dynamic_cast<Window*>(control))
            throw std::invalid_argument("Can't add a window as child.");
    }

    MultiChildControl::~MultiChildControl()
    {
        for (const auto child : children_)
            delete child;
    }

    void MultiChildControl::AddChild(Control* control)
    {
        ControlAddChildCheck(control);

        children_.push_back(control);

        control->SetParent(this);
        control->SetDescendantWindow(GetWindow());

        OnAddChild(control);
    }

    void MultiChildControl::AddChild(Control* control, const int position)
    {
        ControlAddChildCheck(control);

        if (position < 0 || static_cast<decltype(children_.size())>(position) > this->children_.size())
            throw std::invalid_argument("The position is out of range.");

        children_.insert(this->children_.cbegin() + position, control);

        control->SetParent(this);
        control->SetDescendantWindow(GetWindow());

        OnAddChild(control);
    }

    void MultiChildControl::RemoveChild(Control* child)
    {
        const auto i = std::find(this->children_.cbegin(), this->children_.cend(), child);
        if (i == this->children_.cend())
            throw std::invalid_argument("The argument child is not a child of this control.");

        children_.erase(i);

        child->SetParent(nullptr);
        child->SetDescendantWindow(nullptr);

        OnRemoveChild(child);
    }

    void MultiChildControl::RemoveChild(const int position)
    {
        if (position < 0 || static_cast<decltype(this->children_.size())>(position) >= this->children_.size())
            throw std::invalid_argument("The position is out of range.");

        const auto i = children_.cbegin() + position;
        const auto child = *i;

        children_.erase(i);

        child->SetParent(nullptr);
        child->SetDescendantWindow(nullptr);

        OnRemoveChild(child);
    }

    void MultiChildControl::OnAddChild(Control* child)
    {

    }

    void MultiChildControl::OnRemoveChild(Control* child)
    {

    }

    std::list<Control*> GetAncestorList(Control* control)
    {
        std::list<Control*> l;
        while (control != nullptr)
        {
            l.push_front(control);
            control = control->GetParent();
        }
        return l;
    }

    Control* FindLowestCommonAncestor(Control * left, Control * right)
    {
        if (left == nullptr || right == nullptr)
            return nullptr;

        auto&& left_list = GetAncestorList(left);
        auto&& right_list = GetAncestorList(right);

        // the root is different
        if (left_list.front() != right_list.front())
            return nullptr;

        // find the last same control or the last control (one is ancestor of the other)
        auto left_i = left_list.cbegin();
        auto right_i = right_list.cbegin();
        while (true)
        {
            if (left_i == left_list.cend())
                return *(--left_i);
            if (right_i == right_list.cend())
                return *(--right_i);
            if (*left_i != *right_i)
                return *(--left_i);
            ++left_i;
            ++right_i;
        }
    }
}
