#include <cmath>

#include <APG/graphics/Camera.hpp>
#include <APG/input/SDLInputManager.hpp>

#include "systems/PlayerInputSystem.hpp"
#include "systems/ToastSystem.hpp"

#include <easylogging++.h>

#include <glm/detail/type_mat4x4.hpp>

#include <components/CarriedComponent.hpp>
#include <components/DeathComponent.hpp>
#include <components/TowerUpgradeComponent.hpp>

#include <Ashley/Ashley.hpp>

namespace obelisk {

PlayerInputSystem::PlayerInputSystem(int64_t priority, APG::SDLInputManager *inputManager, APG::Camera *camera,
									 std::shared_ptr<ObeliskState> state) :
		IteratingSystem(ashley::Family::getFor({typeid(PositionComponent), typeid(ClickableComponent)}), priority),
		inputManager{inputManager},
		camera{camera},
		state{std::move(state)} {
}

void PlayerInputSystem::update(float deltaTime) {
	if (state->timeToDraw > 0.0f) {
		state->timeToDraw -= deltaTime;
	}

	mouseWorldX = inputManager->getMouseX();
	mouseWorldY = inputManager->getMouseY();

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
	const auto tower = towerMapper.get(entity);

	if (tower != nullptr) {
		if (towerUpgradeMapper.has(entity)) {
			state->toastSystem->addToast("UPGRADE NOT FINISHED", position->position);
			return false;
		}
		el::Loggers::getLogger("obelisk")->info("Card added to %v", tower->name);

		auto carriedEntity = state->heldItem;

		auto carryable = carryableMapper.get(carriedEntity);

		if ((carryable->upgradeType == UpgradeType::LEVEL || carryable->upgradeType == UpgradeType::SPEED) &&
			tower->upgradeType == UpgradeType::NONE) {
			state->toastSystem->addToast("NO WEAPON TO UPGRADE", position->position);
			return false;
		}

		carriedEntity->remove<CarriedComponent>();
		carriedEntity->add<DeathComponent>();
		state->heldItem = nullptr;

		entity->add<TowerUpgradeComponent>(carryable->upgradeType);
		state->toastSystem->addToast("UPGRADING", position->position);
		return true;
	}

	const auto trash = trashMapper.get(entity);

	if (trash != nullptr) {
		auto carriedEntity = state->heldItem;
		carriedEntity->add<DeathComponent>();
		state->heldItem = nullptr;
		state->toastSystem->addToast("TRASHED", position->position);
		return true;
	}

	return false;
}

bool
PlayerInputSystem::processPickup(ashley::Entity *entity, PositionComponent *position, ClickableComponent *clickable,
								 float deltaTime) {
	const auto carryable = carryableMapper.get(entity);
	if (carryable != nullptr) {
		entity->add<CarriedComponent>(carryable->smallSprite, position->position);
		state->heldItem = entity;
		return true;
	}

	const auto deck = deckMapper.get(entity);
	if (deck != nullptr) {
		if (state->timeToDraw > 0.0f) {
			const std::string toast = "COOLDOWN: " + std::to_string((int) std::floor(state->timeToDraw)) + " SECS";
			state->toastSystem->addToast(toast, glm::vec2{mouseWorldX, mouseWorldY});
			return true;
		}

		if (state->hand.size() >= 4) {
			state->toastSystem->addToast("HAND ALREADY FULL", glm::vec2{mouseWorldX, mouseWorldY});
			return true;
		}

		if (deck->cards.empty()) {
			state->toastSystem->addToast("DECK EMPTY", glm::vec2{mouseWorldX, mouseWorldY});
			return true;
		}

		auto newCard = engine->addEntity(std::move(deck->cards.front()));
		deck->cards.pop_front();
		state->hand.push_back(newCard);
		state->timeToDraw = state->drawCooldown;
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

void PlayerInputSystem::addedToEngine(ashley::Engine &engine) {
	this->engine = &engine;
	IteratingSystem::addedToEngine(engine);
}

void PlayerInputSystem::removedFromEngine(ashley::Engine &engine) {
	this->engine = nullptr;
	IteratingSystem::removedFromEngine(engine);
}

}
