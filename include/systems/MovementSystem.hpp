#ifndef OBELISK_MOVEMENTSYSTEM_HPP
#define OBELISK_MOVEMENTSYSTEM_HPP

#include <Ashley/systems/IteratingSystem.hpp>
#include <components/PositionComponent.hpp>
#include <components/MoveComponent.hpp>
#include <Ashley/core/ComponentMapper.hpp>
#include <Ashley/core/Engine.hpp>

namespace obelisk {

class MovementSystem final : public ashley::IteratingSystem, ashley::EntityListener {
public:
	explicit MovementSystem(int64_t priority);

	void processEntity(ashley::Entity *entity, float deltaTime) override;

	void addedToEngine(ashley::Engine &engine) override;

	void removedFromEngine(ashley::Engine &engine) override;

	void entityAdded(ashley::Entity &entity) {}
	void entityRemoved(ashley::Entity &entity) override ;

private:
	ashley::ComponentMapper<PositionComponent> positionMapper{ashley::ComponentMapper<PositionComponent>::getMapper()};
	ashley::ComponentMapper<MoveComponent> moveMapper{ashley::ComponentMapper<MoveComponent>::getMapper()};
};

}

#endif
