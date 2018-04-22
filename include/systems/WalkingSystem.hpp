#ifndef OBELISK_WALKINGSYSTEM_HPP
#define OBELISK_WALKINGSYSTEM_HPP

#include <cstdint>

#include <unordered_map>

#include <Ashley/systems/IteratingSystem.hpp>
#include <Ashley/core/ComponentMapper.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include "ObeliskState.hpp"
#include "components/PositionComponent.hpp"
#include "components/WalkerComponent.hpp"

namespace obelisk {

class WalkingSystem final : public ashley::IteratingSystem {
public:
	explicit WalkingSystem(int64_t priority, std::shared_ptr<ObeliskState> state);

	void processEntity(ashley::Entity *entity, float deltaTime) override;

private:
	ashley::ComponentMapper<PositionComponent> positionMapper = ashley::ComponentMapper<PositionComponent>::getMapper();
	ashley::ComponentMapper<WalkerComponent> walkerMapper = ashley::ComponentMapper<WalkerComponent>::getMapper();

	void handleStep(ashley::Entity *entity, WalkerComponent *walker, float deltaTime);
	void initMaps();

	std::shared_ptr<ObeliskState> state;

	std::unordered_map<glm::ivec2, glm::ivec2> cameFrom;
	std::unordered_map<glm::ivec2, glm::ivec2> wentTo;
};

}

#endif
