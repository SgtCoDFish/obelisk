#include <limits>

#include <Ashley/core/Family.hpp>
#include <Ashley/core/Engine.hpp>

#include "systems/TowerAttackSystem.hpp"

#include <glm/glm.hpp>
#include <easylogging++.h>
#include <components/AttackComponent.hpp>
#include <components/RenderableComponent.hpp>
#include <components/MoveComponent.hpp>

namespace obelisk {

TowerAttackSystem::TowerAttackSystem(int64_t priority, APG::SpriteBase *gunAttackSprite,
									 APG::SpriteBase *rocketAttackSprite) :
		IteratingSystem(ashley::Family::getFor({typeid(PositionComponent), typeid(TowerComponent)}), priority),
		gunAttackSprite{gunAttackSprite},
		rocketAttackSprite{rocketAttackSprite} {
}

void TowerAttackSystem::processEntity(ashley::Entity *entity, float deltaTime) {
	const auto position = positionMapper.get(entity);
	const auto tower = towerMapper.get(entity);

	tower->timeToAttack -= deltaTime;

	if (tower->timeToAttack <= 0.0f) {
		doAttack(position, tower);
	}
}

void TowerAttackSystem::doAttack(PositionComponent *position, TowerComponent *tower) {
	auto logger = el::Loggers::getLogger("obelisk");

	if (tower->upgradeType == UpgradeType::NONE) {
		logger->verbose(9, "Tower has no weapon; not attacking");
		return;
	}

	float shortestDistance = std::numeric_limits<float>::infinity();
	size_t shortestIndex{0u};

	for (int i = 0; i < (int) monsterEntities->size(); ++i) {
		const auto distance = glm::distance(position->position, positionMapper.get(monsterEntities->at(i))->position);
		if (distance < shortestDistance) {
			shortestDistance = distance;
			shortestIndex = i;
		}
	}

	logger->verbose(9, "Chose mob #%v for attack", shortestIndex);

	if (shortestDistance > tower->range) {
		logger->verbose(9, "No mob is in range");
		tower->timeToAttack = tower->attackCooldown / 2.0f;
		return;
	}

	logger->verbose(9, "Performing attack");
	tower->timeToAttack = tower->attackCooldown;

	auto attackEntity = getEngine()->addEntity();

	int attackDamage;
	float moveDuration;
	if (tower->upgradeType == UpgradeType::TOWER_ROCKET_UPGRADE) {
		attackEntity->add<RenderableComponent>(rocketAttackSprite);
		moveDuration = 2.5f;
		attackDamage = 10 + 2 * tower->level;
	} else /*if(tower->upgradeType == UpgradeType::TOWER_GUN_UPGRADE)*/ {
		attackEntity->add<RenderableComponent>(gunAttackSprite);
		moveDuration = 1.0f;
		attackDamage = 3 + tower->level;
	}
	attackEntity->add<PositionComponent>(position->position);
	attackEntity->add<MoveComponent>(monsterEntities->at(shortestIndex), moveDuration, true);
	attackEntity->add<AttackComponent>(attackDamage);
}

void TowerAttackSystem::addedToEngine(ashley::Engine &engine) {
	IteratingSystem::addedToEngine(engine);
	monsterEntities = engine.getEntitiesFor(monsterFamily);
}

void TowerAttackSystem::removedFromEngine(ashley::Engine &engine) {
	IteratingSystem::removedFromEngine(engine);
	monsterEntities = nullptr;
}

}