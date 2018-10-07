#include "cursor.h"

#include "exception.h"

namespace cru::ui
{
    Cursor::Cursor(HCURSOR handle, const bool auto_release)
        : handle_(handle), auto_release_(auto_release)
    {

    }

    Cursor::~Cursor()
    {
        if (auto_release_)
            ::DestroyCursor(handle_);
    }

    std::unordered_map<String, Cursor::Ptr> cursors;
}
