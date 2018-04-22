#ifndef OBELISK_CARRYABLECOMPONENT_HPP
#define OBELISK_CARRYABLECOMPONENT_HPP

#include <Ashley/core/Component.hpp>

namespace APG {
class SpriteBase;
}

namespace obelisk {
enum class UpgradeType {
	NONE,
	TOWER_GUN_UPGRADE,
	TOWER_ROCKET_UPGRADE
};

class CarryableComponent final : public ashley::Component {
public:
	explicit CarryableComponent(APG::SpriteBase *smallSprite) :
			smallSprite{smallSprite} {
	}

	explicit CarryableComponent(APG::SpriteBase *smallSprite, UpgradeType upgradeType, APG::SpriteBase *upgradeSprite)
			:
			smallSprite{smallSprite},
			upgradeType{upgradeType},
			upgradeSprite{upgradeSprite} {
	}

	APG::SpriteBase *smallSprite;

	UpgradeType upgradeType{UpgradeType::NONE};
	APG::SpriteBase *upgradeSprite{nullptr};
};

}

#endif
