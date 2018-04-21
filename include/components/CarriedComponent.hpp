#ifndef OBELISK_CARRIEDCOMPONENT_HPP
#define OBELISK_CARRIEDCOMPONENT_HPP

#include <Ashley/core/Component.hpp>

namespace APG {
class SpriteBase;
}

namespace obelisk {

class CarriedComponent final : public ashley::Component {
public:
	explicit CarriedComponent(APG::SpriteBase *smallSprite, glm::vec2 originalPosition) :
			smallSprite{smallSprite},
			originalPosition{std::move(originalPosition)} {
	}

	APG::SpriteBase *smallSprite;
	glm::vec2 originalPosition;
};

}

#endif
