#ifndef OBELISK_MOVECOMPONENT_HPP
#define OBELISK_MOVECOMPONENT_HPP

#include <utility>
#include <Ashley/core/Component.hpp>
#include <glm/vec2.hpp>

namespace obelisk {

class MoveComponent final : public ashley::Component {
public:
	explicit MoveComponent(glm::vec2 target, float duration) : target{std::move(target)}, duration{duration} {}

	glm::vec2 target;
	float duration;
	float elapsed{0.0f};
};

}

#endif
