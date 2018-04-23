#ifndef OBELISK_TOWERATTACKSYSTEM_HPP
#define OBELISK_TOWERATTACKSYSTEM_HPP

#include <Ashley/systems/IteratingSystem.hpp>
#include <Ashley/core/ComponentMapper.hpp>

#include <components/PositionComponent.hpp>
#include <components/TowerComponent.hpp>
#include <components/MonsterComponent.hpp>
#include <APG/graphics/SpriteBase.hpp>

namespace obelisk {

class TowerAttackSystem final : public ashley::IteratingSystem {
public:
	explicit TowerAttackSystem(int64_t priority, APG::SpriteBase *gunAttackSprite, APG::SpriteBase *rocketAttackSprite);

	void processEntity(ashley::Entity *entity, float deltaTime) override;

	void addedToEngine(ashley::Engine &engine) override;

	void removedFromEngine(ashley::Engine &engine) override;

private:
	ashley::ComponentMapper<PositionComponent> positionMapper{ashley::ComponentMapper<PositionComponent>::getMapper()};
	ashley::ComponentMapper<TowerComponent> towerMapper{ashley::ComponentMapper<TowerComponent>::getMapper()};

	ashley::Family *monsterFamily{ashley::Family::getFor({typeid(PositionComponent), typeid(MonsterComponent)})};

	std::vector<ashley::Entity *> *monsterEntities{nullptr};

	void doAttack(PositionComponent *position, TowerComponent *tower);

	APG::SpriteBase *gunAttackSprite;
	APG::SpriteBase *rocketAttackSprite;
};

}

#endif
