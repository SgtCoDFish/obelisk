#include <cstdint>

#include <array>
#include <vector>
#include <unordered_map>

#include <Ashley/core/Entity.hpp>
#include <Ashley/core/Family.hpp>

#include "ObeliskState.hpp"
#include "systems/WalkingSystem.hpp"

#include <glm/vec2.hpp>
#include <glm/integer.hpp>

#include "easylogging++.h"

namespace obelisk {

WalkingSystem::WalkingSystem(int64_t priority, std::shared_ptr<ObeliskState> state) :
		IteratingSystem(ashley::Family::getFor({typeid(PositionComponent), typeid(WalkerComponent)}), priority),
		state{std::move(state)} {
	initMaps();
}

void WalkingSystem::initMaps() {
	auto logger = el::Loggers::getLogger("obelisk");

	cameFrom[state->traversibleStart] = glm::ivec2{-1, -1};
	// find next destination
	std::vector<glm::ivec2> frontier;
	frontier.push_back(state->traversibleStart);

	constexpr const std::array<glm::ivec2, 4> moves{{{-1, 0}, {1, 0}, {0, -1}, {0, 1}}};

	glm::ivec2 last{-1, -1};

	while (!frontier.empty()) {
		glm::ivec2 current = frontier.back();
		last = current;
		frontier.pop_back();

		logger->verbose(9, "Current: (%v, %v)", current.x, current.y);

		for (const auto &m : moves) {
			const auto newPos = current + m;
			logger->verbose(9, "Trying (%v, %v)", newPos.x, newPos.y);

			if (newPos.x >= state->traversibleWidthInTiles || newPos.y >= state->traversibleHeightInTiles) {
				logger->verbose(9, "\tOOB");
				continue;
			}

			const bool traversible = state->traversibleTiles[newPos.y * state->traversibleWidthInTiles + newPos.x];

			if (!traversible) {
				logger->verbose(9, "\tNot traversible");
				continue;
			}

			if (cameFrom.find(newPos) != cameFrom.end()) {
				logger->verbose(9, "\tAlready visited");
				continue;
			}

			frontier.push_back(newPos);
			cameFrom[newPos] = current;
			wentTo[current] = newPos;
			logger->verbose(9, "cameFrom[(%v, %v)] = (%v, %v)", newPos.x, newPos.y, current.x, current.y);
			logger->verbose(9, "wentTo[(%v, %v)] = (%v, %v)", current.x, current.y, newPos.x, newPos.y);
		}
	}

	if (last == glm::ivec2{-1, -1}) {
		logger->error("Couldn't create a path for walker");
		return;
	}

}

void WalkingSystem::processEntity(ashley::Entity *entity, float deltaTime) {
	auto walker = walkerMapper.get(entity);

	walker->timeToStep -= deltaTime;

	if (walker->timeToStep <= 0.0f) {
		handleStep(entity, walker, deltaTime);
	}
}

void WalkingSystem::handleStep(ashley::Entity *entity, WalkerComponent *walker, float deltaTime) {
	auto logger = el::Loggers::getLogger("obelisk");

	logger->verbose(9, "Step!");
	walker->timeToStep = walker->stepTime;

	auto position = positionMapper.get(entity);
	// 1. find tile of entity
	// 2. get wentTo for that tile
	// 3. start moving towards the wentTo
}

}
