#pragma once

#include <map>

#include "base.h"
#include "application.h"
#include "timer.h"
#include "builder.h"

namespace cru::ui::animations
{
    class Animation;


    class AnimationManager : public Object
    {
    public:
        static AnimationManager* GetInstance()
        {
            return Application::GetInstance()->GetAnimationManager();
        }

    public:
        AnimationManager();
        AnimationManager(const AnimationManager& other) = delete;
        AnimationManager(AnimationManager&& other) = delete;
        AnimationManager& operator=(const AnimationManager& other) = delete;
        AnimationManager& operator=(AnimationManager&& other) = delete;
        ~AnimationManager() override;

        void AddAnimation(Animation* animation);
        void RemoveAnimation(Animation* animation);

    private:
        std::map<String, Animation*> animations_;
        std::shared_ptr<ITimerTask> timer_;
        std::shared_ptr<Action<>> timer_action_;
    };

    class Animation : public Object
    {
        friend class AnimationManager;
    protected:
        Animation(
            String tag,
            double duration,
            const Vector<std::shared_ptr<Action<Animation*, double>>>& step_handlers,
            const Vector<std::shared_ptr<Action<Animation*>>>& start_handlers,
            const Vector<std::shared_ptr<Action<Animation*>>>& finish_handlers,
            const Vector<std::shared_ptr<Action<Animation*>>>& cancel_handlers
        );

    public:
        Animation(const Animation& other) = delete;
        Animation(Animation&& other) = delete;
        Animation& operator=(const Animation& other) = delete;
        Animation& operator=(Animation&& other) = delete;
        ~Animation() override = default; // The animation will never destroy by users.

        bool Step(double time);
        void Cancel();
        String GetTag() const
        {
            return tag_;
        }

    private:
        const String tag_;
        const double duration_;
        Vector<std::shared_ptr<Action<Animation*, double>>> step_handlers_;
        Vector<std::shared_ptr<Action<Animation*>>> start_handlers_;
        Vector<std::shared_ptr<Action<Animation*>>> finish_handlers_;
        Vector<std::shared_ptr<Action<Animation*>>> cancel_handlers_;

        double current_time_ = 0;

    public:
        class Builder : public OneTimeBuilder<Animation>
        {
        public:
            Builder(String tag, const double duration)
                : tag(std::move(tag)), duration(duration)
            {

            }

            String tag;
            double duration;

            Builder& AddStepHandler(Action<Animation*, double>&& handler)
            {
                if (IsValid())
                    step_handlers_.push_back(std::make_shared<Action<Animation*, double>>(std::move(handler)));
                return *this;
            }

            Builder& AddStartHandler(Action<Animation*>&& handler)
            {
                if (IsValid())
                    start_handlers_.push_back(std::make_shared<Action<Animation*>>(std::move(handler)));
                return *this;
            }

            Builder& AddFinishHandler(Action<Animation*>&& handler)
            {
                if (IsValid())
                    finish_handlers_.push_back(std::make_shared<Action<Animation*>>(std::move(handler)));
                return *this;
            }

            Builder& AddCancelHandler(Action<Animation*>&& handler)
            {
                if (IsValid())
                    cancel_handlers_.push_back(std::make_shared<Action<Animation*>>(std::move(handler)));
                return *this;
            }

        protected:
            Animation* OnCreate() override
            {
                return new Animation(std::move(tag), duration, step_handlers_, start_handlers_, finish_handlers_, cancel_handlers_);
            }

        private:
            Vector<std::shared_ptr<Action<Animation*, double>>> step_handlers_;
            Vector<std::shared_ptr<Action<Animation*>>> start_handlers_;
            Vector<std::shared_ptr<Action<Animation*>>> finish_handlers_;
            Vector<std::shared_ptr<Action<Animation*>>> cancel_handlers_;
        };
    };
}
