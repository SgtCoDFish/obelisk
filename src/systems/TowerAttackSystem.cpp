#include <Ashley/core/Family.hpp>

#include "systems/TowerAttackSystem.hpp"

namespace obelisk {

TowerAttackSystem::TowerAttackSystem(int64_t priority) :
		IteratingSystem(ashley::Family::getFor({typeid(PositionComponent), typeid(TowerComponent)}), priority) {
}

void TowerAttackSystem::processEntity(ashley::Entity * entity, float deltaTime) {
	const auto position = positionMapper.get(entity);
	const auto tower = towerMapper.get(entity);

	tower->timeToAttack -= deltaTime;

	if(tower->timeToAttack <= 0.0f) {
		doAttack(position, tower);
	}
}

void TowerAttackSystem::doAttack(PositionComponent *position, TowerComponent *tower) {
	tower->timeToAttack = tower->attackCooldown;
}

}