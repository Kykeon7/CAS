#pragma once
#include <unordered_map>
#include <typeindex>
#include <functional>

namespace cas {
class EventBus {
public:
	template<typename Event>
	using Callback = std::function<void(const Event&)>;

	template<typename Event>
	void suscribe(Callback<Event> callback) {
		auto& sucribers = m_suscribers[typeid(Event)];

		sucribers.emplace_back(
			[callback](const void* event) {
				callback(*static_cast<const Event*>(event));
			}
		);
	}

	template<typename Event>
	void dispatch(const Event& event) {
		auto& it = m_suscribers.find(typeid(Event));
		if (it == m_suscribers.end())
			return;

		for (auto& suscrier : it->second)
			suscriber(&event);
	}

private:
	std::unordered_map<std::type_index,
		std::vector<std::function<void(const void*)>>
	> m_suscribers;
};
}


