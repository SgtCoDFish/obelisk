#include <utility>

#include <Ashley/core/Family.hpp>
#include <easylogging++.h>
#include <ObeliskState.hpp>

#include "systems/TowerUpgradeSystem.hpp"

namespace obelisk {

TowerUpgradeSystem::TowerUpgradeSystem(int64_t priority, std::shared_ptr<ObeliskState> state,
									   APG::SpriteBase *gunUpgrade,
									   APG::SpriteBase *rocketUpgrade,
									   APG::SpriteBase *constructionOverlay) :
		IteratingSystem(ashley::Family::getFor(
				{typeid(PositionComponent), typeid(RenderableComponent), typeid(TowerComponent),
				 typeid(TowerUpgradeComponent)}), priority),
		state{std::move(state)},
		gunUpgrade{gunUpgrade},
		rocketUpgrade{rocketUpgrade},
		constructionOverlay{constructionOverlay} {
}

void TowerUpgradeSystem::processEntity(ashley::Entity *entity, float deltaTime) {
	auto tower = towerMapper.get(entity);
	auto towerUpgrade = towerUpgradeMapper.get(entity);
	auto renderable = renderableMapper.get(entity);

	if (!towerUpgrade->applied) {
		towerUpgrade->applied = true;

		if(tower->upgradeType != UpgradeType::NONE && renderable->secondary != nullptr) {
			towerUpgrade->secondarySprite = renderable->secondary;
			towerUpgrade->secondarySpritePos = renderable->secondaryPos;
		}

		renderable->setSecondary(constructionOverlay);
	}

	towerUpgrade->timeRemaining -= deltaTime;

	if (towerUpgrade->timeRemaining <= 0.0f) {
		const auto position = positionMapper.get(entity);
		if (towerUpgrade->upgradeType == UpgradeType::TOWER_GUN_UPGRADE) {
			state->toastSystem->addToast("GUN UPGRADE COMPLETE", position->position);
			renderable->setSecondary(gunUpgrade);
			tower->upgradeType = towerUpgrade->upgradeType;
			tower->range = renderable->sprite->getWidth() * 4;
			tower->attackCooldown = 2.0f;
		} else if (towerUpgrade->upgradeType == UpgradeType::TOWER_ROCKET_UPGRADE) {
			state->toastSystem->addToast("ROCKET UPGRADE COMPLETE", position->position);
			renderable->setSecondary(rocketUpgrade);
			tower->upgradeType = towerUpgrade->upgradeType;
			tower->range = renderable->sprite->getWidth() * 9;
			tower->attackCooldown = 7.5f;
		} else if (towerUpgrade->upgradeType == UpgradeType::LEVEL) {
			state->toastSystem->addToast("LEVEL UPGRADE COMPLETE", position->position);
			tower->level++;
			renderable->setSecondary(towerUpgrade->secondarySprite);
		} else if (towerUpgrade->upgradeType == UpgradeType::SPEED) {
			state->toastSystem->addToast("SPEED UPGRADE COMPLETE", position->position);
			tower->timeToAttack *= 0.75f;
			renderable->setSecondary(towerUpgrade->secondarySprite);
		} else {
			el::Loggers::getLogger("obelisk")->error("Unknown tower upgrade type");
		}

		entity->remove<TowerUpgradeComponent>();
	}
}

}
