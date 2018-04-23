#include "systems/MovementSystem.hpp"
#include "Ashley/core/Family.hpp"

#include <glm/glm.hpp>
#include <components/DeathComponent.hpp>
#include <easylogging++.h>

namespace obelisk {

MovementSystem::MovementSystem(int64_t priority) :
		IteratingSystem(ashley::Family::getFor({typeid(PositionComponent), typeid(MoveComponent)}), priority) {

}

void MovementSystem::processEntity(ashley::Entity *entity, float deltaTime) {
	const auto position = positionMapper.get(entity);
	const auto move = moveMapper.get(entity);

	if (move->hasEntityTarget) {
		if(move->entityTarget == nullptr) {
			entity->add<DeathComponent>();
			return;
		}

		move->target = positionMapper.get(move->entityTarget)->position;
	}

	move->elapsed += deltaTime;
	if (move->elapsed >= move->duration || glm::distance(position->position, move->target) <= 5) {
		position->position = move->target;
		entity->remove<MoveComponent>();

		if (move->dieOnImpact) {
			entity->add<DeathComponent>();
		}

		return;
	}

	position->position = glm::mix(position->position, move->target, move->elapsed / move->duration);
}

void MovementSystem::addedToEngine(ashley::Engine &engine) {
	IteratingSystem::addedToEngine(engine);
	engine.addEntityListener(this);
}

void MovementSystem::removedFromEngine(ashley::Engine &engine) {
	engine.removeEntityListener(this);
	IteratingSystem::removedFromEngine(engine);
}

void MovementSystem::entityRemoved(ashley::Entity &entity) {
	// avoid segfaulting if we're targeting an entity which gets destroyed
	for (const auto &e : *entities) {
		const auto move = moveMapper.get(e);

		if (move->entityTarget == &entity) {
			move->entityTarget = nullptr;
		}
	}
}

}