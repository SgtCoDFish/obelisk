#include <utility>

#include "components/RenderableComponent.hpp"

namespace obelisk {

RenderableComponent::RenderableComponent(APG::SpriteBase *sprite) :
		sprite{sprite} {
}

RenderableComponent::RenderableComponent(APG::SpriteBase *sprite, APG::SpriteBase *secondary) :
		sprite{sprite},
		secondary{secondary},
		secondaryPos{(sprite->getWidth() - secondary->getWidth()) / 2,
					 (sprite->getHeight() - secondary->getWidth()) / 2} {
}

}