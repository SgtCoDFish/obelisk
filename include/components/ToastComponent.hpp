#ifndef OBELISK_TOASTCOMPONENT_HPP
#define OBELISK_TOASTCOMPONENT_HPP

#include <string>
#include <utility>

#include <Ashley/core/Component.hpp>
#include <glm/vec2.hpp>

namespace obelisk {

class ToastComponent final : public ashley::Component {
public:
	explicit ToastComponent(std::string text, float duration, glm::vec2 initialPosition) :
			text{std::move(text)},
			duration{duration},
			timeRemaining{duration},
			initialPosition{std::move(initialPosition)} {
	}

	std::string text;
	float duration;
	float timeRemaining;
	glm::vec2 initialPosition;
};

}

#endif
