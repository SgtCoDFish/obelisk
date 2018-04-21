#ifndef OBELISK_RENDERABLECOMPONENT_HPP
#define OBELISK_RENDERABLECOMPONENT_HPP

#include <Ashley/core/Component.hpp>
#include <APG/graphics/SpriteBase.hpp>

namespace obelisk {

class RenderableComponent final : public ashley::Component {
public:
	explicit RenderableComponent(APG::SpriteBase * sprite);

	APG::SpriteBase * sprite;
};

}

#endif
