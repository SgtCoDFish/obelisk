#ifndef OBELISK_RENDERABLECOMPONENT_HPP
#define OBELISK_RENDERABLECOMPONENT_HPP

#include <Ashley/core/Component.hpp>
#include <APG/graphics/SpriteBase.hpp>

#include <glm/vec2.hpp>

namespace obelisk {

class RenderableComponent final : public ashley::Component {
public:
	explicit RenderableComponent(APG::SpriteBase *sprite);

	explicit RenderableComponent(APG::SpriteBase *sprite, APG::SpriteBase *secondary);

	APG::SpriteBase *sprite;

	APG::SpriteBase *secondary{nullptr};
	glm::vec2 secondaryPos{-1.0f, -1.0f};

	bool visible{true};

	void setSecondary(APG::SpriteBase *secondary);
	void calculateSecondaryPos();
};

}

#endif
