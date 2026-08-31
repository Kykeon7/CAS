#pragma once
#include <unordered_map>
#include <typeindex>
#include <functional>
#include <vector>

namespace cas {

class EventBus {
public:
    template<typename Event>
    using Callback = std::function<void(const Event&)>;

    template<typename Event>
    void subscribe(Callback<Event> callback) {
        auto& subscribers = m_subscribers[typeid(Event)];

        subscribers.emplace_back(
            [callback](const void* event) {
                callback(*static_cast<const Event*>(event));
            }
        );
    }

    template<typename Event>
    void dispatch(const Event& event) {
        auto it = m_subscribers.find(typeid(Event));
        if (it == m_subscribers.end())
            return;

        for (auto& subscriber : it->second)
            subscriber(&event);
    }   

private:
    std::unordered_map<
        std::type_index,
        std::vector<std::function<void(const void*)>>
    > m_subscribers;
};

} // namespace cas

