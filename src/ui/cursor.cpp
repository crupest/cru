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

    namespace cursors
    {
        Cursor::Ptr arrow{};
        Cursor::Ptr hand{};
        Cursor::Ptr i_beam{};
    }
}
