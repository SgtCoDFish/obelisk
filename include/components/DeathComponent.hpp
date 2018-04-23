#ifndef OBELISK_DEATHCOMPONENT_HPP
#define OBELISK_DEATHCOMPONENT_HPP

#include <Ashley/core/Component.hpp>

namespace obelisk {

class DeathComponent final : public ashley::Component {
public:
	explicit DeathComponent(bool processAttack = false) : processAttack{processAttack} {}

	bool processAttack;
};

}

#endif
