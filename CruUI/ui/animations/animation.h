#pragma once

#include <unordered_map>

#include "base.h"
#include "application.h"
#include "timer.h"

namespace cru::ui::animations
{
    using AnimationTimeUnit = FloatSecond;

    
    using IAnimationDelegate = ICancelable;
    using AnimationDelegatePtr = CancelablePtr;

    using AnimationStepHandlerPtr = FunctionPtr<void(AnimationDelegatePtr, double)>;
    using AnimationStartHandlerPtr = FunctionPtr<void(AnimationDelegatePtr)>;


    namespace details
    {
        class Animation;
        using AnimationPtr = std::unique_ptr<Animation>;

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

            AnimationDelegatePtr CreateAnimation(
                String tag,
                AnimationTimeUnit duration,
                Vector<AnimationStepHandlerPtr> step_handlers,
                Vector<AnimationStartHandlerPtr> start_handlers,
                Vector<ActionPtr> finish_handlers,
                Vector<ActionPtr> cancel_handlers
            );
            void RemoveAnimation(const String& tag);

        private:
            void SetTimer();
            void KillTimer();

        private:
            std::unordered_map<String, AnimationPtr> animations_;
            std::shared_ptr<ICancelable> timer_;
            ActionPtr timer_action_;
        };
    }

    class AnimationBuilder : public Object
    {
    public:
        AnimationBuilder(String tag, const AnimationTimeUnit duration)
            : tag(std::move(tag)), duration(duration)
        {

        }

        String tag;
        AnimationTimeUnit duration;

        AnimationBuilder& AddStepHandler(AnimationStepHandlerPtr handler)
        {
            step_handlers_.push_back(std::move(handler));
            return *this;
        }

        AnimationBuilder& AddStartHandler(AnimationStartHandlerPtr handler)
        {
            start_handlers_.push_back(std::move(handler));
            return *this;
        }

        AnimationBuilder& AddFinishHandler(ActionPtr handler)
        {
            finish_handlers_.push_back(std::move(handler));
            return *this;
        }

        AnimationBuilder& AddCancelHandler(ActionPtr handler)
        {
            cancel_handlers_.push_back(std::move(handler));
            return *this;
        }

        AnimationDelegatePtr Start() const;

    private:
        Vector<AnimationStepHandlerPtr> step_handlers_;
        Vector<AnimationStartHandlerPtr> start_handlers_;
        Vector<ActionPtr> finish_handlers_;
        Vector<ActionPtr> cancel_handlers_;
    };
}
