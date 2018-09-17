#include "animation.h"

#include <cassert>
#include <utility>

namespace cru::ui::animations
{
    constexpr int frame_rate = 60;
    constexpr double frame_step_time = 1.0 / frame_rate;

    AnimationManager::AnimationManager()
        : timer_action_(new Action<>([this]()
    {
        for (auto& animation : animations_)
        {
            if (animation.second->Step(frame_step_time))
                InvokeLater([=]
            {
                RemoveAnimation(animation.second); //TODO!!!
            });
        }
    }))
    {

    }

    AnimationManager::~AnimationManager()
    {
        for (auto& animation : animations_)
            delete animation.second;

        if (timer_)
            timer_->Cancel();
    }

    void AnimationManager::AddAnimation(Animation* animation)
    {
        if (animations_.empty())
            timer_ = SetInterval(frame_step_time, timer_action_);

        const auto find_result = animations_.find(animation->GetTag());
        if (find_result != animations_.cend())
            find_result->second->Cancel();
        animations_.insert_or_assign(animation->GetTag(), animation);
    }

    void AnimationManager::RemoveAnimation(Animation* animation)
    {
        const auto find_result = animations_.find(animation->GetTag());
        if (find_result != animations_.cend())
        {
            delete find_result->second;
            animations_.erase(find_result);
        }

        if (animations_.empty())
        {
            assert(timer_);
            timer_->Cancel();
            timer_ = nullptr;
        }
    }

    Animation::Animation(String tag, const double duration,
        const Vector<std::shared_ptr<Action<Animation*, double>>>& step_handlers,
        const Vector<std::shared_ptr<Action<Animation*>>>& start_handlers,
        const Vector<std::shared_ptr<Action<Animation*>>>& finish_handlers,
        const Vector<std::shared_ptr<Action<Animation*>>>& cancel_handlers
    ) : tag_(std::move(tag)), duration_(duration), step_handlers_(step_handlers),
        start_handlers_(start_handlers), finish_handlers_(finish_handlers), cancel_handlers_(cancel_handlers)
    {
        AnimationManager::GetInstance()->AddAnimation(this);
    }

    bool Animation::Step(const double time)
    {
        current_time_ += time;
        if (current_time_ > duration_)
        {
            for (auto& handler : step_handlers_)
                (*handler)(this, 1);
            for (auto& handler : finish_handlers_)
                (*handler)(this);
            return true;
        }
        else
        {
            for (auto& handler : step_handlers_)
                (*handler)(this, current_time_ / duration_);
            return false;
        }
    }

    void Animation::Cancel()
    {
        for (auto& handler : cancel_handlers_)
            (*handler)(this);
        InvokeLater([this]
        {
            AnimationManager::GetInstance()->RemoveAnimation(this); //TODO!!!
        });
    }
}
