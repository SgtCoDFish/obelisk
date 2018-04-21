#ifndef OBELISK_POSITIONCOMPONENT_HPP
#define OBELISK_POSITIONCOMPONENT_HPP

#include <Ashley/core/Component.hpp>
#include <glm/vec2.hpp>

namespace obelisk {

class PositionComponent final : public ashley::Component {
public:
	explicit PositionComponent(float x, float y);
	explicit PositionComponent(glm::vec2 pos);

	glm::vec2 position;
};

}

#endif
