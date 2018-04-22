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
#include <components/DeathComponent.hpp>

#include "easylogging++.h"

namespace obelisk {

WalkingSystem::WalkingSystem(int64_t priority, std::shared_ptr<ObeliskState> state) :
		IteratingSystem(ashley::Family::getFor({typeid(PositionComponent), typeid(WalkerComponent)}), priority),
		state{std::move(state)} {
	initMaps();
}

void WalkingSystem::initMaps() {
	auto logger = el::Loggers::getLogger("obelisk");
	auto map = state->currentMap;

	cameFrom[map->traversibleStart] = glm::ivec2{-1, -1};
	// find next destination
	std::vector<glm::ivec2> frontier;
	frontier.push_back(map->traversibleStart);

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

			if (!map->isTraversible(newPos.x, newPos.y)) {
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

	auto map = state->currentMap;

	auto rendererPos = map->renderer->getPosition();
	auto entityMapPos = position->position - rendererPos;
	auto entityTilePos = glm::ivec2{static_cast<int>(entityMapPos.x / map->getTileWidth()),
									static_cast<int>(entityMapPos.y / map->getTileHeight())};

	logger->info("Entity is at %v, %v", entityTilePos.x, entityTilePos.y);

	if (entityTilePos == map->traversibleEnd) {
		entity->add<DeathComponent>();
		return;
	}

	auto nextTileIt = wentTo.find(entityTilePos);

	glm::ivec2 nextTile{-1, -1};

	if (nextTileIt != wentTo.end()) {
		nextTile = nextTileIt->second;
	} else {
		nextTile = map->traversibleStart;
	}

	logger->info("Next: %v, %v", nextTile.x, nextTile.y);

	position->position = rendererPos + glm::vec2{nextTile.x * map->getTileWidth(), nextTile.y * map->getTileHeight()};
	// 1. find tile of entity
	// 2. get wentTo for that tile
	// 3. start moving towards the wentTo
}

}
