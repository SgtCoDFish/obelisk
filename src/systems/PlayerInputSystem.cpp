#include <APG/graphics/Camera.hpp>
#include <APG/input/SDLInputManager.hpp>

#include "systems/PlayerInputSystem.hpp"

#include <easylogging++.h>

#include <glm/detail/type_mat4x4.hpp>
#include <components/CarriedComponent.hpp>
#include <components/DeathComponent.hpp>

namespace obelisk {

PlayerInputSystem::PlayerInputSystem(int64_t priority, APG::SDLInputManager *inputManager, APG::Camera *camera,
									 ObeliskState *state) :
		IteratingSystem(ashley::Family::getFor({typeid(PositionComponent), typeid(ClickableComponent)}), priority),
		inputManager{inputManager},
		camera{camera},
		state{state} {
}

void PlayerInputSystem::update(float deltaTime) {
	mouseWorldX = inputManager->getMouseX();
	mouseWorldY = inputManager->getMouseY();

	//el::Loggers::getLogger("obelisk")->info("(x, y) = (%v, %v)", mouseWorldX, mouseWorldY);

	if (inputManager->isLeftMouseJustPressed()) {
		for (auto &e : *entities) {
			const auto &position = positionMapper.get(e);
			const auto &clickable = clickableMapper.get(e);

			if (isInside(position, clickable)) {
				bool result = false;

				if (state->heldItem == nullptr) {
					result = processPickup(e, position, clickable, deltaTime);
				} else {
					result = processDrop(e, position, clickable, deltaTime);
				}

				if (result) {
					break;
				}
			}
		}
	} else if (inputManager->isRightMouseJustPressed()) {
		if (state->heldItem != nullptr) {
			processUnpickup(state->heldItem, deltaTime);
		}
	}
}

void PlayerInputSystem::processEntity(ashley::Entity *entity, float deltaTime) {
}

bool PlayerInputSystem::processDrop(ashley::Entity *entity, PositionComponent *position, ClickableComponent *clickable,
									float deltaTime) {
	// entity is the _target_
	el::Loggers::getLogger("obelisk")->info("processing potential tower");
	const auto tower = towerMapper.get(entity);

	if (tower != nullptr) {
		// TODO: not this
		tower->level += 1;
		el::Loggers::getLogger("obelisk")->info("added to %v, now level %v", tower->name, tower->level);

		auto carriedEntity = state->heldItem;
		carriedEntity->remove<CarriedComponent>();
		carriedEntity->add<DeathComponent>();
		state->heldItem = nullptr;
		return false;
	}

	return false;
}

bool
PlayerInputSystem::processPickup(ashley::Entity *entity, PositionComponent *position, ClickableComponent *clickable,
								 float deltaTime) {
	auto carryable = carryableMapper.get(entity);
	if (carryable != nullptr) {
		entity->add<CarriedComponent>(carryable->smallSprite, position->position);
		state->heldItem = entity;
		return true;
	}

	return false;
}

void PlayerInputSystem::processUnpickup(ashley::Entity *entity, float deltaTime) {
	const auto &position = positionMapper.get(entity);
	const auto &clickable = clickableMapper.get(entity);

	auto carried = ashley::ComponentMapper<CarriedComponent>::getMapper().get(entity);

	position->position = carried->originalPosition;

	entity->remove<CarriedComponent>();
	state->heldItem = nullptr;
}

bool PlayerInputSystem::isInside(PositionComponent *position, ClickableComponent *clickable) {
	const float intersectX1 = (position->position.x + clickable->clickRect.x) / camera->getZoom();
	const float intersectY1 = (position->position.y + clickable->clickRect.y) / camera->getZoom();

	const float intersectX2 =
			(position->position.x + clickable->clickRect.x + clickable->clickRect.w) / camera->getZoom();
	const float intersectY2 =
			(position->position.y + clickable->clickRect.y + clickable->clickRect.h) / camera->getZoom();

	return (mouseWorldX >= intersectX1 && mouseWorldX < intersectX2 && mouseWorldY >= intersectY1 &&
			mouseWorldY < intersectY2);
}

}
