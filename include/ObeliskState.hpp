#ifndef OBELISK_OBELISKSTATE_HPP
#define OBELISK_OBELISKSTATE_HPP

#include <chrono>
#include <random>

#include <Ashley/core/Entity.hpp>

#include <glm/vec2.hpp>
#include <random>
#include "ObeliskMap.hpp"

namespace obelisk {
class ToastSystem;
class EntitySpawnSystem;

struct ObeliskState {
	explicit ObeliskState() {
		hand.reserve(4);
	}

	ashley::Entity *heldItem{nullptr};
	ashley::Entity *upgradeDeck{nullptr};
	ObeliskMap *currentMap{nullptr};
	std::mt19937_64 rand{static_cast<unsigned long>(
								 std::chrono::high_resolution_clock::now().time_since_epoch().count()
						 )};
	ToastSystem *toastSystem{nullptr};
	EntitySpawnSystem *entitySpawnSystem{nullptr};

	std::vector<ashley::Entity *> hand;

	int killCount{0};

	bool paused{true};

	float drawCooldown{6.0f};
	float timeToDraw{1.0f};

	int screenWidth{0};
	int screenHeight{0};
};

}

#endif
