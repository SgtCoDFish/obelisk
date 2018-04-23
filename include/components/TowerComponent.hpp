#ifndef OBELISK_TOWERCOMPONENT_HPP
#define OBELISK_TOWERCOMPONENT_HPP

#include <string>
#include <utility>

#include <Ashley/core/Component.hpp>
#include <UpgradeType.hpp>

namespace obelisk {

class TowerComponent final : public ashley::Component {
public:
	explicit TowerComponent(std::string name) :
			name{std::move(name)} {
	}

	std::string name;

	UpgradeType upgradeType{UpgradeType::NONE};
	int level{0};
	float range{64 * 5};

	float attackCooldown{10.0f};
	float timeToAttack{10.0f};
};

}

#endif
