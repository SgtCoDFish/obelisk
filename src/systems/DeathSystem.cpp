#include <cstdint>

#include <utility>
#include <algorithm>

#include <Ashley/core/Engine.hpp>
#include <Ashley/core/Family.hpp>
#include <components/CardComponent.hpp>
#include <components/AttackComponent.hpp>

#include "components/DeathComponent.hpp"
#include "systems/DeathSystem.hpp"
#include "ObeliskState.hpp"

namespace obelisk {

obelisk::DeathSystem::DeathSystem(int64_t priority, std::shared_ptr<ObeliskState> state) :
		IteratingSystem(ashley::Family::getFor({typeid(DeathComponent)}), priority),
		state{std::move(state)} {
}

void obelisk::DeathSystem::processEntity(ashley::Entity *entity, float deltaTime) {
	auto cardMapper = ashley::ComponentMapper<CardComponent>::getMapper();

	if (cardMapper.has(entity)) {
		state->hand.erase(std::remove(state->hand.begin(), state->hand.end(), entity));
	}

	auto attackMapper = ashley::ComponentMapper<AttackComponent>::getMapper();
	auto attack = attackMapper.get(entity);
	if (attack != nullptr) {
		el::Loggers::getLogger("obelisk")->info("Would do %v", attack->damage);
	}

	this->getEngine()->removeEntity(entity);
}
}