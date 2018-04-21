#include <Ashley/Ashley.hpp>
#include <APG/graphics/SpriteBatch.hpp>
#include <components/CarriedComponent.hpp>

#include "systems/RenderSystem.hpp"

#include "components/PositionComponent.hpp"
#include "components/RenderableComponent.hpp"

namespace obelisk {

RenderSystem::RenderSystem(APG::SpriteBatch *batch, int64_t priority) :
		IteratingSystem(ashley::Family::getFor({typeid(PositionComponent), typeid(RenderableComponent)}), priority),
		batch{batch} {

}

void RenderSystem::processEntity(ashley::Entity *entity, float deltaTime) {
	auto position = ashley::ComponentMapper<PositionComponent>::getMapper().get(entity);
	auto renderable = ashley::ComponentMapper<RenderableComponent>::getMapper().get(entity);

	if (renderable->visible) {
		auto sprite = renderable->sprite;

		auto carried = ashley::ComponentMapper<CarriedComponent>::getMapper().get(entity);
		if (carried != nullptr) {
			sprite = carried->smallSprite;
		}

		batch->draw(sprite, position->position.x, position->position.y);
	}
}

void RenderSystem::update(float deltaTime) {
	batch->begin();
	IteratingSystem::update(deltaTime);
	batch->end();
}

}
