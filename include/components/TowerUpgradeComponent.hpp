#ifndef OBELISK_TOWERUPGRADECOMPONENT_HPP
#define OBELISK_TOWERUPGRADECOMPONENT_HPP

#include <Ashley/core/Component.hpp>

#include "UpgradeType.hpp"
#include <APG/graphics/SpriteBase.hpp>
#include <glm/vec2.hpp>

namespace obelisk {

class TowerUpgradeComponent final : public ashley::Component {
public:
	explicit TowerUpgradeComponent(UpgradeType upgradeType) :
			upgradeType{upgradeType} {
		switch (upgradeType) {
			case UpgradeType::LEVEL: {
				timeRequired = 5.0f;
				break;
			}

			case UpgradeType ::SPEED: {
				timeRequired = 5.0f;
			}

			case UpgradeType::TOWER_ROCKET_UPGRADE: {
				timeRequired = 20.0f;
				break;
			}
			case UpgradeType::TOWER_GUN_UPGRADE: {
				timeRequired = 10.0f;
				break;
			}

			case UpgradeType::NONE: {
				el::Loggers::getLogger("obelisk")->error("NONE upgrade type was created");
				break;
			}
		}

		timeRemaining = timeRequired;
	}

	UpgradeType upgradeType;

	float timeRequired;
	float timeRemaining;

	bool applied{false};

	APG::SpriteBase *secondarySprite{nullptr};
	glm::vec2 secondarySpritePos{-1.0f, -1.0f};
};

}

#endif
