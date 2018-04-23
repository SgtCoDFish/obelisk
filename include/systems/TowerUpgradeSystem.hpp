#ifndef OBELISK_TOWERUPGRADESYSTEM_HPP
#define OBELISK_TOWERUPGRADESYSTEM_HPP

#include <memory>

#include <Ashley/systems/IteratingSystem.hpp>
#include <Ashley/core/ComponentMapper.hpp>
#include <components/TowerComponent.hpp>
#include <components/PositionComponent.hpp>
#include <components/TowerUpgradeComponent.hpp>
#include <components/RenderableComponent.hpp>

#include "ObeliskState.hpp"

namespace obelisk {

class TowerUpgradeSystem final : public ashley::IteratingSystem {
public:
	explicit TowerUpgradeSystem(int64_t priority, std::shared_ptr<ObeliskState> state, APG::SpriteBase *gunUpgrade,
								APG::SpriteBase *rocketUpgrade, APG::SpriteBase *constructionOverlay);


	void processEntity(ashley::Entity *entity, float deltaTime) override;

private:
	ashley::ComponentMapper<PositionComponent> positionMapper{ashley::ComponentMapper<PositionComponent>::getMapper()};
	ashley::ComponentMapper<RenderableComponent> renderableMapper{
			ashley::ComponentMapper<RenderableComponent>::getMapper()};

	ashley::ComponentMapper<TowerComponent> towerMapper{ashley::ComponentMapper<TowerComponent>::getMapper()};

	ashley::ComponentMapper<TowerUpgradeComponent> towerUpgradeMapper{
			ashley::ComponentMapper<TowerUpgradeComponent>::getMapper()};

	std::shared_ptr<ObeliskState> state;

	APG::SpriteBase *gunUpgrade;
	APG::SpriteBase *rocketUpgrade;
	APG::SpriteBase *constructionOverlay;
};

}

#endif
