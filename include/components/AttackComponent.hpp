#ifndef OBELISK_ATTACKCOMPONENT_HPP
#define OBELISK_ATTACKCOMPONENT_HPP

#include <Ashley/core/Component.hpp>
#include <Ashley/core/Entity.hpp>

namespace obelisk {

class AttackComponent final : public ashley::Component {
public:
	explicit AttackComponent(ashley::Entity *target, int damage) : target{target}, damage{damage} {}

	ashley::Entity *target;
	int damage;
};

}

#endif
