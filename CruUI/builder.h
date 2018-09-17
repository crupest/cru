#pragma once

#include "base.h"

namespace cru
{
    template<typename T>
    class OneTimeBuilder : public Object
    {
    protected:
        OneTimeBuilder() = default;

    public:
        OneTimeBuilder(const OneTimeBuilder& other) = delete;
        OneTimeBuilder(OneTimeBuilder&& other) = delete;
        OneTimeBuilder& operator=(const OneTimeBuilder& other) = delete;
        OneTimeBuilder& operator=(OneTimeBuilder&& other) = delete;
        virtual ~OneTimeBuilder() = default;

        T* Create()
        {
            if (is_valid_)
            {
                is_valid_ = false;
                return OnCreate();
            }
            else
                throw std::runtime_error("OneTimeBuilder is invalid.");
        }

        bool IsValid() const
        {
            return is_valid_;
        }

    protected:
        virtual T* OnCreate() = 0;

    private:
        bool is_valid_ = true;
    };
}
