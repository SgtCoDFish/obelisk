#include <Ashley/Ashley.hpp>
#include <APG/graphics/SpriteBatch.hpp>
#include <components/CarriedComponent.hpp>

#include "systems/RenderSystem.hpp"

namespace obelisk {

RenderSystem::RenderSystem(APG::SpriteBatch *batch, int64_t priority) :
		IteratingSystem(ashley::Family::getFor({typeid(PositionComponent), typeid(RenderableComponent)}), priority),
		batch{batch} {

}

void RenderSystem::processEntity(ashley::Entity *entity, float deltaTime) {
	auto position = positionMapper.get(entity);
	auto renderable = renderableMapper.get(entity);
	auto color = colorMapper.get(entity);

	if (renderable->visible) {
		auto sprite = renderable->sprite;

		bool renderSmall = true;

		auto carried = ashley::ComponentMapper<CarriedComponent>::getMapper().get(entity);
		if (carried != nullptr) {
			sprite = carried->smallSprite;
			renderSmall = false;
		}

		if (color != nullptr) {
			auto origColor = batch->getColor();
			batch->setColor(color->color);
			batch->draw(sprite, position->position.x, position->position.y);
			if (renderable->secondary != nullptr && renderSmall) {
				batch->draw(renderable->secondary, renderable->secondaryPos.x + position->position.x,
							renderable->secondaryPos.y + position->position.y);
			}
			batch->setColor(origColor);
		} else {
			batch->draw(sprite, position->position.x, position->position.y);
			if (renderable->secondary != nullptr && renderSmall) {
				batch->draw(renderable->secondary, renderable->secondaryPos.x + position->position.x,
							renderable->secondaryPos.y + position->position.y);
			}
		}

		auto annotation = annotationMapper.get(entity);
		if (annotation != nullptr) {
			batch->draw(annotation->sprite, position->position.x,
						position->position.y + renderable->sprite->getHeight() + 10);
		}
	}
}

void RenderSystem::update(float deltaTime) {
	batch->begin();
	IteratingSystem::update(deltaTime);
	batch->end();
}

}
