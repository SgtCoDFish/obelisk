#include <Ashley/core/Family.hpp>

#include "systems/TowerUpgradeSystem.hpp"

namespace obelisk {

TowerUpgradeSystem::TowerUpgradeSystem(int64_t priority) :
		IteratingSystem(ashley::Family::getFor({typeid(TowerComponent), typeid(TowerUpgradeComponent)}), priority) {
}

void TowerUpgradeSystem::processEntity(ashley::Entity *entity, float deltaTime) {
	auto tower = towerMapper.get(entity);
	auto towerUpgrade = towerUpgradeMapper.get(entity);

	towerUpgrade->timeRemaining -= deltaTime;

	tower->hasWeapon = true;
	tower->level++;

	entity->remove<TowerUpgradeComponent>();
}

}
