#include "systems/MovementSystem.hpp"
#include "Ashley/core/Family.hpp"

#include <glm/glm.hpp>

namespace obelisk {

MovementSystem::MovementSystem(int64_t priority) :
		IteratingSystem(ashley::Family::getFor({typeid(PositionComponent), typeid(MoveComponent)}), priority) {

}

void MovementSystem::processEntity(ashley::Entity *entity, float deltaTime) {
	const auto position = positionMapper.get(entity);
	const auto move = moveMapper.get(entity);

	move->elapsed += deltaTime;
	if (move->elapsed >= move->duration) {
		position->position = move->target;
		entity->remove<MoveComponent>();
		return;
	}

	position->position = glm::mix(position->position, move->target, move->elapsed / move->duration);
}
}