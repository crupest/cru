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

    private:
        HCURSOR handle_;
        bool auto_release_;
    };


    extern std::unordered_map<String, Cursor::Ptr> cursors;
    constexpr auto cursor_arrow_key = L"System_Arrow";
    constexpr auto cursor_hand_key = L"System_Hand";
    constexpr auto cursor_i_beam_key = L"System_IBeam";
}
