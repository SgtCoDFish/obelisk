#ifndef OBELISK_TOWERUPGRADESYSTEM_HPP
#define OBELISK_TOWERUPGRADESYSTEM_HPP

#include <Ashley/systems/IteratingSystem.hpp>
#include <Ashley/core/ComponentMapper.hpp>
#include <components/TowerComponent.hpp>
#include <components/TowerUpgradeComponent.hpp>

namespace obelisk {

class TowerUpgradeSystem final : public ashley::IteratingSystem {
public:
	explicit TowerUpgradeSystem(int64_t priority);

	void processEntity(ashley::Entity *entity, float deltaTime) override;

private:
	ashley::ComponentMapper<TowerComponent> towerMapper{ashley::ComponentMapper<TowerComponent>::getMapper()};
	ashley::ComponentMapper<TowerUpgradeComponent> towerUpgradeMapper{ashley::ComponentMapper<TowerUpgradeComponent>::getMapper()};
};

}

#endif
