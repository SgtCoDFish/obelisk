#include <Ashley/core/Family.hpp>
#include <Ashley/core/Engine.hpp>
#include <components/DeathComponent.hpp>

#include "systems/ToastSystem.hpp"

namespace obelisk {

ToastSystem::ToastSystem(int64_t priority, APG::FontManager *fontManager, APG::FontManager::font_handle fontHandle,
						 APG::FontManager::font_handle bigFontHandle) :
		IteratingSystem(ashley::Family::getFor({typeid(PositionComponent), typeid(ToastComponent)}), priority),
		fontManager{fontManager}, font{std::move(fontHandle)}, bigFont{std::move(bigFontHandle)} {
}

void ToastSystem::addToast(const std::string &text, glm::vec2 position) {
	auto entity = engine->addEntity();

	entity->add<PositionComponent>(position.x, position.y);
	entity->add<ToastComponent>(text, 1.0f, position);
	auto sprite = fontManager->renderText(font, text, true, APG::FontRenderMethod::NICE);

	entity->add<RenderableComponent>(sprite);
}

void ToastSystem::addBigToast(const std::string &text, glm::vec2 position) {
	auto entity = engine->addEntity();

	entity->add<PositionComponent>(position.x, position.y);
	entity->add<ToastComponent>(text, 1.0f, position);
	auto sprite = fontManager->renderText(bigFont, text, true, APG::FontRenderMethod::NICE);

	entity->add<RenderableComponent>(sprite);
}

void ToastSystem::addedToEngine(ashley::Engine &engine) {
	this->engine = &engine;
	IteratingSystem::addedToEngine(engine);
}

void ToastSystem::removedFromEngine(ashley::Engine &engine) {
	this->engine = nullptr;
	IteratingSystem::removedFromEngine(engine);
}

void ToastSystem::processEntity(ashley::Entity *entity, float deltaTime) {
	const auto toast = toastMapper.get(entity);

	toast->timeRemaining -= deltaTime;

	if (toast->timeRemaining <= 0.0f) {
		entity->add<DeathComponent>();
		return;
	}

	const auto targetPositionY = glm::mix(toast->initialPosition.y,
										  toast->initialPosition.y - 25,
										  (toast->duration - toast->timeRemaining) / toast->duration);

	const auto pos = positionMapper.get(entity);
	pos->position.y = targetPositionY;
}

}