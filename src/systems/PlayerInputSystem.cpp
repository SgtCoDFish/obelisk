#include <APG/graphics/Camera.hpp>
#include <APG/input/SDLInputManager.hpp>

#include "systems/PlayerInputSystem.hpp"

#include <easylogging++.h>

#include <glm/detail/type_mat4x4.hpp>

namespace obelisk {

PlayerInputSystem::PlayerInputSystem(int64_t priority, APG::SDLInputManager *inputManager, APG::Camera *camera) :
		IteratingSystem(ashley::Family::getFor({typeid(PositionComponent), typeid(ClickableComponent)}), priority),
		inputManager{inputManager},
		camera{camera} {
}

void PlayerInputSystem::update(float deltaTime) {
	mouseWorldX = inputManager->getMouseX();
	mouseWorldY = inputManager->getMouseY();

	if (inputManager->isLeftMouseJustPressed()) {
		IteratingSystem::update(deltaTime);
	}
}

void PlayerInputSystem::processEntity(ashley::Entity *entity, float deltaTime) {
	const auto &position = positionMapper.get(entity);
	const auto &clickable = clickableMapper.get(entity);

	const float intersectX1 = position->position.x + clickable->clickRect.x;
	const float intersectY1 = position->position.y + clickable->clickRect.y;
	const float intersectX2 = intersectX1 + clickable->clickRect.w / camera->getZoom();
	const float intersectY2 = intersectY1 + clickable->clickRect.h / camera->getZoom();

	if (mouseWorldX >= intersectX1 && mouseWorldX < intersectX2 && mouseWorldY >= intersectY1 &&
		mouseWorldY < intersectY2) {
		el::Loggers::getLogger("obelisk")->info("click");
	}
}

}
