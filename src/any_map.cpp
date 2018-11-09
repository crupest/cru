#include "any_map.h"

namespace cru
{
    AnyMap::ListenerToken AnyMap::RegisterValueChangeListener(const String& key, const Listener& listener)
    {
        const auto token = current_listener_token_++;
        map_[key].second.push_back(token);
        listeners_.emplace(token, listener);
        return token;
    }

    void AnyMap::UnregisterValueChangeListener(const ListenerToken token)
    {
        const auto find_result = listeners_.find(token);
        if (find_result != listeners_.cend())
            listeners_.erase(find_result);
    }

    void AnyMap::InvokeListeners(std::list<ListenerToken>& listener_list, const std::any& value)
    {
        auto i = listener_list.cbegin();
        while (i != listener_list.cend())
        {
            auto current_i = i++;
            const auto find_result = listeners_.find(*current_i);
            if (find_result != listeners_.cend())
                find_result->second(value);
            else
                listener_list.erase(current_i); // otherwise remove the invalid listener token.
        }
    }
}
