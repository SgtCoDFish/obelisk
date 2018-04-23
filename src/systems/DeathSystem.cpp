#include <cstdint>

#include <utility>
#include <algorithm>
#include <array>

#include <Ashley/core/Engine.hpp>
#include <Ashley/core/Family.hpp>
#include <components/MonsterComponent.hpp>
#include <components/IntensificationComponent.hpp>
#include "systems/DeathSystem.hpp"
#include "systems/ToastSystem.hpp"
#include "ObeliskState.hpp"

namespace obelisk {

DeathSystem::DeathSystem(int64_t priority, std::shared_ptr<ObeliskState> state) :
		IteratingSystem(ashley::Family::getFor({typeid(DeathComponent)}), priority),
		state{std::move(state)} {
}

void DeathSystem::processEntity(ashley::Entity *entity, float deltaTime) {
	if (cardMapper.has(entity)) {
		state->hand.erase(std::remove(state->hand.begin(), state->hand.end(), entity));
	}

	auto death = deathMapper.get(entity);

	if (death->processAttack) {
		auto attack = attackMapper.get(entity);
		if (attack != nullptr && attack->target != nullptr && !attack->target->hasComponent<DeathComponent>()) {
			auto monster = attack->target->getComponent<MonsterComponent>();
			monster->hp -= attack->damage;

			if (monster->hp <= 0) {
				auto monsterPos = positionMapper.get(attack->target);
				constexpr std::array<const char *, 4> deathToasts {{"URK", "SKREE", "EEK", "URGH"}};
				std::uniform_int_distribution<int> distribution{0, 3};
				state->toastSystem->addToast(deathToasts[distribution(state->rand)], monsterPos->position);
				attack->target->add<DeathComponent>();
				state->killCount += 1;

				if (state->killCount % 3 == 0) {
					const auto intensifyEntity = getEngine()->addEntity();
					intensifyEntity->add<IntensificationComponent>(state->killCount);
				}
			}
		}
	} else if(death->takeALife) {
		state->lives -= 1;
	}

	this->getEngine()->removeEntity(entity);
}

void DeathSystem::addedToEngine(ashley::Engine &engine) {
	IteratingSystem::addedToEngine(engine);
	engine.addEntityListener(this);
}

void DeathSystem::removedFromEngine(ashley::Engine &engine) {
	engine.removeEntityListener(this);
	IteratingSystem::removedFromEngine(engine);
}

void DeathSystem::entityRemoved(ashley::Entity &entity) {
	for (const auto &e : *entities) {
		auto attack = attackMapper.get(e);

		if (attack != nullptr && attack->target == &entity) {
			e->remove<AttackComponent>();
		}
	}
}

}