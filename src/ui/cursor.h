#pragma once

#include "system_headers.h"
#include <memory>
#include <unordered_map>

#include "base.h"

namespace cru::ui
{
    class Cursor : public Object
    {
    public:
        using Ptr = std::shared_ptr<Cursor>;

        Cursor(HCURSOR handle, bool auto_release);
        Cursor(const Cursor& other) = delete;
        Cursor(Cursor&& other) = delete;
        Cursor& operator=(const Cursor& other) = delete;
        Cursor& operator=(Cursor&& other) = delete;
        ~Cursor() override;

        HCURSOR GetHandle() const
        {
            return handle_;
        }

    private:
        HCURSOR handle_;
        bool auto_release_;
    };

    namespace cursors
    {
        extern Cursor::Ptr arrow;
        extern Cursor::Ptr hand;
        extern Cursor::Ptr i_beam;        
    }
}
