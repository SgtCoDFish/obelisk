#ifndef OBELISK_MOVECOMPONENT_HPP
#define OBELISK_MOVECOMPONENT_HPP

#include <utility>
#include <Ashley/core/Component.hpp>
#include <Ashley/core/Entity.hpp>
#include <glm/vec2.hpp>

namespace obelisk {

class MoveComponent final : public ashley::Component {
public:
	explicit MoveComponent(glm::vec2 target, float duration) : target{std::move(target)}, duration{duration} {}

	explicit MoveComponent(ashley::Entity *target, float duration, bool die) : hasEntityTarget{true},
																			   entityTarget{target}, duration{duration},
																			   dieOnImpact{die} {}

	bool hasEntityTarget{false};
	ashley::Entity *entityTarget;
	glm::vec2 target{-1.0f, -1.0f};
	float duration;
	float elapsed{0.0f};
	bool dieOnImpact{false};

};

}

#endif
