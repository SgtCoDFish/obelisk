#ifndef OBELISK_DEATHCOMPONENT_HPP
#define OBELISK_DEATHCOMPONENT_HPP

#include <Ashley/core/Component.hpp>
#include <glm/vec2.hpp>

namespace obelisk {

class DeathComponent final : public ashley::Component {
public:
	explicit DeathComponent(bool processAttack = false) : processAttack{processAttack} {}

	// this is a super hacky way to get a death component for monsters to do damage to the player.
	explicit DeathComponent(glm::vec2 dummy) : processAttack{false}, takeALife{true} {}

	bool processAttack;
	bool takeALife{false};
};

}

#endif
