#ifndef OBELISK_TOWERUPGRADECOMPONENT_HPP
#define OBELISK_TOWERUPGRADECOMPONENT_HPP

#include <Ashley/core/Component.hpp>

namespace obelisk {

class TowerUpgradeComponent final : public ashley::Component {
public:
	explicit TowerUpgradeComponent(float timeRequired) :
			timeRequired{timeRequired},
			timeRemaining{timeRequired} {
	}

	const float timeRequired;
	float timeRemaining;
};

}

#endif
