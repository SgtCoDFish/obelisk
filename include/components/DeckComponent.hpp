#ifndef OBELISK_DECKCOMPONENT_HPP
#define OBELISK_DECKCOMPONENT_HPP

#include <deque>
#include <memory>

#include <Ashley/core/Component.hpp>

namespace obelisk {

class DeckComponent final : public ashley::Component {
public:
	explicit DeckComponent() = default;

	std::deque<std::unique_ptr<ashley::Entity>> cards;
};

}

#endif
