#ifndef OBELISK_DECKCOMPONENT_HPP
#define OBELISK_DECKCOMPONENT_HPP

#include <forward_list>
#include <memory>

#include <Ashley/core/Component.hpp>

namespace obelisk {

class DeckComponent final : public ashley::Component {
public:
	explicit DeckComponent() = default;

	std::forward_list<std::unique_ptr<ashley::Entity>> cards;
};

}

#endif
