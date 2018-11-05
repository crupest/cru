#pragma once

#include <unordered_map>

#include "base.h"
#include "application.h"
#include "timer.h"

namespace cru::ui::animations
{
    using AnimationTimeUnit = FloatSecond;

    struct IAnimationDelegate : virtual Interface
    {
        virtual void Cancel() = 0;
    };

    using AnimationDelegatePtr = std::shared_ptr<IAnimationDelegate>;

    using AnimationStepHandler = std::function<void(AnimationDelegatePtr, double)>;
    using AnimationStartHandler = std::function<void(AnimationDelegatePtr)>;
    using AnimationFinishHandler = std::function<void()>;
    using AnimationCancelHandler = std::function<void()>;

    namespace details
    {
        class Animation;
        using AnimationPtr = std::unique_ptr<Animation>;

        class AnimationInfo
        {
        public:
            AnimationInfo(String tag, const AnimationTimeUnit duration)
                : tag(std::move(tag)),
                  duration(duration)
            {

            }
            AnimationInfo(const AnimationInfo& other) = default;
            AnimationInfo(AnimationInfo&& other) = default;
            AnimationInfo& operator=(const AnimationInfo& other) = default;
            AnimationInfo& operator=(AnimationInfo&& other) = default;
            ~AnimationInfo() = default;

            String tag;
            AnimationTimeUnit duration;
            std::vector<AnimationStepHandler> step_handlers{};
            std::vector<AnimationStartHandler> start_handlers{};
            std::vector<AnimationFinishHandler> finish_handlers{};
            std::vector<AnimationCancelHandler> cancel_handlers{};
        };

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

            AnimationDelegatePtr CreateAnimation(AnimationInfo info);
            void RemoveAnimation(const String& tag);

        private:
            void SetTimer();
            void KillTimer();

        private:
            std::unordered_map<String, AnimationPtr> animations_;
            std::optional<TimerTask> timer_;
        };
    }

    class AnimationBuilder : public Object
    {
    public:
        AnimationBuilder(String tag, const AnimationTimeUnit duration)
            : info_(std::move(tag), duration)
        {

        }

        AnimationBuilder& AddStepHandler(const AnimationStepHandler& handler)
        {
            CheckValid();
            info_.step_handlers.push_back(handler);
            return *this;
        }

        AnimationBuilder& AddStartHandler(const AnimationStartHandler& handler)
        {
            CheckValid();
            info_.start_handlers.push_back(handler);
            return *this;
        }

        AnimationBuilder& AddFinishHandler(const AnimationFinishHandler& handler)
        {
            CheckValid();
            info_.finish_handlers.push_back(handler);
            return *this;
        }

        AnimationBuilder& AddCancelHandler(const AnimationCancelHandler& handler)
        {
            CheckValid();
            info_.cancel_handlers.push_back(handler);
            return *this;
        }

        AnimationDelegatePtr Start();

    private:
        void CheckValid() const
        {
            if (!valid_)
                throw std::runtime_error("The animation builder is invalid.");
        }

        bool valid_ = true;
        details::AnimationInfo info_;
    };
}
