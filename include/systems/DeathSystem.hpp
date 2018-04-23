#ifndef OBELISK_DEATHSYSTEM_HPP
#define OBELISK_DEATHSYSTEM_HPP

#include <cstdint>
#include <memory>

#include <Ashley/core/ComponentMapper.hpp>
#include <Ashley/systems/IteratingSystem.hpp>
#include "ObeliskState.hpp"

#include <components/CardComponent.hpp>
#include <components/AttackComponent.hpp>
#include <components/PositionComponent.hpp>

#include "components/DeathComponent.hpp"

namespace obelisk {

class DeathSystem final : public ashley::IteratingSystem, ashley::EntityListener {
public:
	explicit DeathSystem(int64_t priority, std::shared_ptr<ObeliskState> state);

	void processEntity(ashley::Entity *entity, float deltaTime) override;

	void addedToEngine(ashley::Engine &engine) override;

	void removedFromEngine(ashley::Engine &engine) override;

	void entityAdded(ashley::Entity &entity) override {}

	void entityRemoved(ashley::Entity &entity) override;

private:
	std::shared_ptr<ObeliskState> state;

	ashley::ComponentMapper<CardComponent> cardMapper = ashley::ComponentMapper<CardComponent>::getMapper();
	ashley::ComponentMapper<AttackComponent> attackMapper = ashley::ComponentMapper<AttackComponent>::getMapper();
	ashley::ComponentMapper<DeathComponent> deathMapper = ashley::ComponentMapper<DeathComponent>::getMapper();
	ashley::ComponentMapper<PositionComponent> positionMapper = ashley::ComponentMapper<PositionComponent>::getMapper();
};

}

#endif
