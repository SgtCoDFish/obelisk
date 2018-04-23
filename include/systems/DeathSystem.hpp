#ifndef OBELISK_DEATHSYSTEM_HPP
#define OBELISK_DEATHSYSTEM_HPP

#include <cstdint>
#include <memory>

#include <Ashley/systems/IteratingSystem.hpp>
#include "ObeliskState.hpp"

namespace obelisk {

class DeathSystem final : public ashley::IteratingSystem {
public:
	explicit DeathSystem(int64_t priority, std::shared_ptr<ObeliskState> state);

	void processEntity(ashley::Entity *entity, float deltaTime) override;

private:
	std::shared_ptr<ObeliskState> state;
};

}

#endif
