#include "animation.hpp"

#include <utility>

#include "application.hpp"

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
            Animation(AnimationInfo info, AnimationDelegatePtr delegate)
                : info_(std::move(info)), delegate_(std::move(delegate))
            {
                
            }

            Animation(const Animation& other) = delete;
            Animation(Animation&& other) = delete;
            Animation& operator=(const Animation& other) = delete;
            Animation& operator=(Animation&& other) = delete;
            ~Animation() override;


            // If finish or invalid, return false.
            bool Step(AnimationTimeUnit time);

            String GetTag() const
            {
                return info_.tag;
            }

        private:
            const AnimationInfo info_;
            const AnimationDelegatePtr delegate_;

            AnimationTimeUnit current_time_ = AnimationTimeUnit::zero();
        };

        AnimationManager* AnimationManager::GetInstance()
        {
            return Application::GetInstance()->ResolveSingleton<AnimationManager>([](auto)
            {
                return new AnimationManager{};
            });
        }

        AnimationManager::AnimationManager()
        {

        }

        AnimationManager::~AnimationManager()
        {
            KillTimer();
        }

        AnimationDelegatePtr AnimationManager::CreateAnimation(AnimationInfo info)
        {
            if (animations_.empty())
                SetTimer();

            const auto tag = info.tag;
            auto delegate = std::make_shared<AnimationDelegateImpl>(tag);
            animations_[tag] = std::make_unique<Animation>(std::move(info), delegate);

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
            if (!timer_.has_value())
                timer_ = SetInterval(std::chrono::duration_cast<std::chrono::milliseconds>(frame_step_time), [this]()
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
                });
        }

        void AnimationManager::KillTimer()
        {
            if (timer_.has_value())
            {
                timer_.value().Cancel();
                timer_ = std::nullopt;
            }
        }

        Animation::~Animation()
        {
            if (current_time_ < info_.duration)
                for (const auto& handler : info_.cancel_handlers)
                    handler();
        }

        bool Animation::Step(const AnimationTimeUnit time)
        {
            current_time_ += time;
            if (current_time_ > info_.duration)
            {
                for (const auto& handler : info_.step_handlers)
                    handler(delegate_, 1);
                for (const auto& handler : info_.finish_handlers)
                    handler();
                return true;
            }
            else
            {
                for (const auto& handler : info_.step_handlers)
                    handler(delegate_, current_time_ / info_.duration);
                return false;
            }
        }

    }

    AnimationDelegatePtr AnimationBuilder::Start()
    {
        CheckValid();
        valid_ = false;
        return details::AnimationManager::GetInstance()->CreateAnimation(std::move(info_));
    }
}
