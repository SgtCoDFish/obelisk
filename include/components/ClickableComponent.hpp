#ifndef OBELISK_CLICKABLECOMPONENT_HPP
#define OBELISK_CLICKABLECOMPONENT_HPP

#include <utility>

#include <SDL2/SDL_rect.h>

#include <Ashley/core/Component.hpp>

namespace obelisk {

class ClickableComponent final : public ashley::Component {
public:
	explicit ClickableComponent(SDL_Rect clickRect) : clickRect{std::move(clickRect)} {
	}

	SDL_Rect clickRect;
};

}

#endif
