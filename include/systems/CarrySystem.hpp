#ifndef OBELISK_CARRYSYSTEM_HPP
#define OBELISK_CARRYSYSTEM_HPP

#include <Ashley/systems/IteratingSystem.hpp>

namespace APG {
class InputManager;
}

namespace obelisk {

class CarrySystem final : public ashley::IteratingSystem {
public:
	explicit CarrySystem(APG::InputManager *inputManager, int64_t priority);

	void processEntity(ashley::Entity *entity, float deltaTime) override;

private:
	APG::InputManager *inputManager;
};

}

#endif
