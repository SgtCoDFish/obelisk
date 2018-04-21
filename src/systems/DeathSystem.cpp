#include <cstdint>

#include <Ashley/core/Engine.hpp>
#include <Ashley/core/Family.hpp>

#include "components/DeathComponent.hpp"
#include "systems/DeathSystem.hpp"

namespace obelisk {

obelisk::DeathSystem::DeathSystem(int64_t priority) :
		IteratingSystem(ashley::Family::getFor({typeid(DeathComponent)}), priority) {

}

void obelisk::DeathSystem::processEntity(ashley::Entity *entity, float deltaTime) {
	this->getEngine()->removeEntity(entity);
}
}