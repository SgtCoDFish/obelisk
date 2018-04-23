#ifndef OBELISK_MONSTERCOMPONENT_HPP
#define OBELISK_MONSTERCOMPONENT_HPP

#include <Ashley/core/Component.hpp>

namespace obelisk {

class MonsterComponent final : public ashley::Component {
public:
	explicit MonsterComponent(int hp) : maxHP{hp}, hp{hp} {}

	int maxHP;
	int hp;
};

}

#endif
