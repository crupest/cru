#include "animation.h"

#include <cassert>
#include <utility>

namespace cru::ui::animations
{
    namespace details
    {
        class AnimationDelegateImpl;
        constexpr double frame_rate = 60;
        constexpr AnimationTimeUnit frame_step_time = AnimationTimeUnit(1) / frame_rate;

        
        class AnimationDelegateImpl : public virtual IAnimationDelegate
        {
        public:
            explicit AnimationDelegateImpl(String tag)
                : tag_(std::move(tag))
            {

            }
            AnimationDelegateImpl(const AnimationDelegateImpl& other) = delete;
            AnimationDelegateImpl(AnimationDelegateImpl&& other) = delete;
            AnimationDelegateImpl& operator=(const AnimationDelegateImpl& other) = delete;
            AnimationDelegateImpl& operator=(AnimationDelegateImpl&& other) = delete;
            ~AnimationDelegateImpl() override = default;

            void Cancel() override
            {
                AnimationManager::GetInstance()->RemoveAnimation(tag_);
            }

        private:
            String tag_;
        };


        class Animation : public Object
        {
        public:
            Animation(
                String tag,
                AnimationTimeUnit duration,
                Vector<AnimationStepHandlerPtr> step_handlers,
                Vector<AnimationStartHandlerPtr> start_handlers,
                Vector<ActionPtr> finish_handlers,
                Vector<ActionPtr> cancel_handlers,
                AnimationDelegatePtr delegate
            );
            Animation(const Animation& other) = delete;
            Animation(Animation&& other) = delete;
            Animation& operator=(const Animation& other) = delete;
            Animation& operator=(Animation&& other) = delete;
            ~Animation() override;


            // If finish or invalid, return false.
            bool Step(AnimationTimeUnit time);

            String GetTag() const
            {
                return tag_;
            }

        private:
            const String tag_;
            const AnimationTimeUnit duration_;
            Vector<AnimationStepHandlerPtr> step_handlers_;
            Vector<AnimationStartHandlerPtr> start_handlers_;
            Vector<ActionPtr> finish_handlers_;
            Vector<ActionPtr> cancel_handlers_;
            AnimationDelegatePtr delegate_;

            AnimationTimeUnit current_time_ = AnimationTimeUnit::zero();
        };

        AnimationManager::AnimationManager()
            : timer_action_(CreateActionPtr([this]()
        {
            auto i = animations_.cbegin();
            while (i != animations_.cend())
            {
                auto current_i = i++;
                if (current_i->second->Step(frame_step_time))
                    animations_.erase(current_i);
            }

            if (animations_.empty())
                KillTimer();
        }))
        {

        }

        AnimationManager::~AnimationManager()
        {
            KillTimer();
        }

        AnimationDelegatePtr AnimationManager::CreateAnimation(String tag, AnimationTimeUnit duration,
            Vector<AnimationStepHandlerPtr> step_handlers, Vector<AnimationStartHandlerPtr> start_handlers,
            Vector<ActionPtr> finish_handlers, Vector<ActionPtr> cancel_handlers)
        {
            if (animations_.empty())
                SetTimer();

            auto delegate = std::make_shared<AnimationDelegateImpl>(tag);

            animations_[tag] = std::make_unique<Animation>(tag, duration, std::move(step_handlers), std::move(start_handlers), std::move(finish_handlers), std::move(cancel_handlers), delegate);

            return delegate;
        }

        void AnimationManager::RemoveAnimation(const String& tag)
        {
            const auto find_result = animations_.find(tag);
            if (find_result != animations_.cend())
                animations_.erase(find_result);

            if (animations_.empty())
                KillTimer();
        }

        void AnimationManager::SetTimer()
        {
            if (timer_ == nullptr)
                timer_ = SetInterval(std::chrono::duration_cast<std::chrono::milliseconds>(frame_step_time), timer_action_);
        }

        void AnimationManager::KillTimer()
        {
            if (timer_ != nullptr)
            {
                timer_->Cancel();
                timer_ = nullptr;
            }
        }

        Animation::Animation(
            String tag,
            AnimationTimeUnit duration,
            Vector<AnimationStepHandlerPtr> step_handlers,
            Vector<AnimationStartHandlerPtr> start_handlers,
            Vector<ActionPtr> finish_handlers,
            Vector<ActionPtr> cancel_handlers,
            AnimationDelegatePtr delegate
        ) : tag_(std::move(tag)), duration_(duration),
            step_handlers_(std::move(step_handlers)),
            start_handlers_(std::move(start_handlers)),
            finish_handlers_(std::move(finish_handlers)),
            cancel_handlers_(std::move(cancel_handlers)),
            delegate_(std::move(delegate))
        {

        }

        Animation::~Animation()
        {
            if (current_time_ < duration_)
                for (auto& handler : cancel_handlers_)
                    (*handler)();
        }

        bool Animation::Step(const AnimationTimeUnit time)
        {
            current_time_ += time;
            if (current_time_ > duration_)
            {
                for (auto& handler : step_handlers_)
                    (*handler)(delegate_, 1);
                for (auto& handler : finish_handlers_)
                    (*handler)();
                return true;
            }
            else
            {
                for (auto& handler : step_handlers_)
                    (*handler)(delegate_, current_time_ / duration_);
                return false;
            }
        }

    }

    AnimationDelegatePtr AnimationBuilder::Start() const
    {
        return details::AnimationManager::GetInstance()->CreateAnimation(tag, duration, step_handlers_, start_handlers_, finish_handlers_, cancel_handlers_);
    }
}
