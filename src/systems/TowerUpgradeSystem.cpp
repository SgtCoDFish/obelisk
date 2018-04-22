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

	renderable->setSecondary(constructionOverlay);

	towerUpgrade->timeRemaining -= deltaTime;

	if (towerUpgrade->timeRemaining <= 0.0f) {
		const auto position = positionMapper.get(entity);
		state->toastSystem->addToast("UPGRADE COMPLETE", position->position);
		if (towerUpgrade->upgradeType == UpgradeType::TOWER_GUN_UPGRADE) {
			renderable->setSecondary(gunUpgrade);
		} else if (towerUpgrade->upgradeType == UpgradeType::TOWER_ROCKET_UPGRADE) {
			renderable->setSecondary(rocketUpgrade);
		} else {
			el::Loggers::getLogger("obelisk")->error("Unknown tower upgrade type");
		}
		tower->hasWeapon = true;
		tower->level++;

		entity->remove<TowerUpgradeComponent>();
	}
}

}
