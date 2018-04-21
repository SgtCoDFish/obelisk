#include <APG/input/InputManager.hpp>
#include <components/PositionComponent.hpp>
#include <components/CarriedComponent.hpp>

#include <Ashley/core/Family.hpp>
#include <Ashley/core/ComponentMapper.hpp>

#include "systems/CarrySystem.hpp"

namespace obelisk {

obelisk::CarrySystem::CarrySystem(APG::InputManager *inputManager, int64_t priority) :
		IteratingSystem(ashley::Family::getFor({typeid(PositionComponent), typeid(CarriedComponent)}), priority),
		inputManager{inputManager} {
}

void CarrySystem::processEntity(ashley::Entity *entity, float deltaTime) {
	const auto position = ashley::ComponentMapper<PositionComponent>::getMapper().get(entity);

	// TODO: dirty hack; *2 because we're half zoomed
	position->position.x = inputManager->getMouseX() * 2;
	position->position.y = inputManager->getMouseY() * 2;
}

}
