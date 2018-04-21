#ifndef OBELISK_CARRYABLECOMPONENT_HPP
#define OBELISK_CARRYABLECOMPONENT_HPP

#include <Ashley/core/Component.hpp>

namespace APG {
class SpriteBase;
}

namespace obelisk {

class CarryableComponent final : public ashley::Component {
public:
	explicit CarryableComponent(APG::SpriteBase *smallSprite) :
			smallSprite{smallSprite} {
	}

	APG::SpriteBase *smallSprite;
};

}

#endif
