#ifndef OBELISK_TOWERUPGRADECOMPONENT_HPP
#define OBELISK_TOWERUPGRADECOMPONENT_HPP

#include <Ashley/core/Component.hpp>

#include "UpgradeType.hpp"

namespace obelisk {

class TowerUpgradeComponent final : public ashley::Component {
public:
	explicit TowerUpgradeComponent(float timeRequired, UpgradeType upgradeType) :
			timeRequired{timeRequired},
			timeRemaining{timeRequired},
			upgradeType{upgradeType}{
	}

	const float timeRequired;
	float timeRemaining;

	UpgradeType upgradeType;
};

}

#endif
