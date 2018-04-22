#include <utility>

#include "components/RenderableComponent.hpp"

namespace obelisk {

RenderableComponent::RenderableComponent(APG::SpriteBase *sprite) :
		sprite{sprite} {
}

RenderableComponent::RenderableComponent(APG::SpriteBase *sprite, APG::SpriteBase *secondary) :
		sprite{sprite},
		secondary{secondary} {
	calculateSecondaryPos();
}

void RenderableComponent::calculateSecondaryPos() {
	secondaryPos = glm::vec2{(sprite->getWidth() - secondary->getWidth()) / 2,
							 (sprite->getHeight() - secondary->getWidth()) / 2};
}

void RenderableComponent::setSecondary(APG::SpriteBase *secondary) {
	this->secondary = secondary;

	if (secondary != nullptr) {
		calculateSecondaryPos();
	}
}

}