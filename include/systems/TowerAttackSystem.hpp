#ifndef OBELISK_TOWERATTACKSYSTEM_HPP
#define OBELISK_TOWERATTACKSYSTEM_HPP

#include <Ashley/systems/IteratingSystem.hpp>
#include <Ashley/core/ComponentMapper.hpp>

#include <components/PositionComponent.hpp>
#include <components/TowerComponent.hpp>

namespace obelisk {

class TowerAttackSystem final : public ashley::IteratingSystem {
public:
	explicit TowerAttackSystem(int64_t priority);

	void processEntity(ashley::Entity * entity, float deltaTime) override;

private:
	ashley::ComponentMapper<PositionComponent> positionMapper{ashley::ComponentMapper<PositionComponent>::getMapper()};
	ashley::ComponentMapper<TowerComponent> towerMapper{ashley::ComponentMapper<TowerComponent>::getMapper()};

	void doAttack(PositionComponent * position, TowerComponent * tower);
};

}

#endif
