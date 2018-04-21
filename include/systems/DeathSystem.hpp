#ifndef OBELISK_DEATHSYSTEM_HPP
#define OBELISK_DEATHSYSTEM_HPP

#include <cstdint>
#include <Ashley/systems/IteratingSystem.hpp>

namespace obelisk {

class DeathSystem final : public ashley::IteratingSystem {
public:
	explicit DeathSystem(int64_t priority);

	void processEntity(ashley::Entity *entity, float deltaTime) override;
};

}

#endif
