#ifndef OBELISK_ATTACKCOMPONENT_HPP
#define OBELISK_ATTACKCOMPONENT_HPP

#include <Ashley/core/Component.hpp>

namespace obelisk {

class AttackComponent final : public ashley::Component {
public:
	explicit AttackComponent(int damage) : damage{damage} {}

	int damage;
};

}

#endif
