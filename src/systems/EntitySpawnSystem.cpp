#include <Ashley/Ashley.hpp>
#include <components/PositionComponent.hpp>
#include <components/RenderableComponent.hpp>
#include <components/WalkerComponent.hpp>
#include <components/MonsterComponent.hpp>

#include "systems/EntitySpawnSystem.hpp"

namespace obelisk {

EntitySpawnSystem::EntitySpawnSystem(int64_t priority, float interval, ObeliskState *state,
									 std::vector<APG::SpriteBase *> sprites, std::vector<MonsterStats> monsterStats) :
		IntervalSystem(interval, priority),
		state{state},
		sprites{std::move(sprites)},
		monsterStats{std::move(monsterStats)} {
	if (this->sprites.size() != this->monsterStats.size()) {
		el::Loggers::getLogger("obelisk")->error("error; monstersprites != monsterstats lengthwise");
	}
}

void EntitySpawnSystem::addedToEngine(ashley::Engine &engine) {
	this->engine = &engine;
}

void EntitySpawnSystem::removedFromEngine(ashley::Engine &engine) {
	this->engine = nullptr;
}

void EntitySpawnSystem::updateInterval() {
	auto newMonster = engine->addEntity();
	auto renderer = state->currentMap->renderer.get();

	auto distribution = std::uniform_int_distribution<>{0, static_cast<int>(sprites.size() - 1)};

	APG::SpriteBase *sprite{nullptr};
	MonsterStats *stats{nullptr};

	while (sprite == nullptr || sprite == lastSprite) {
		const auto choice = distribution(state->rand);
		sprite = sprites[choice];
		stats = &monsterStats[choice];
	}

	lastSprite = sprite;

	auto renderable = std::make_unique<RenderableComponent>(sprite);

	newMonster->add<PositionComponent>(renderer->getPosition().x + renderer->getPixelWidth(),
									   renderer->getPosition().y +
									   (renderer->getPixelHeight() - sprite->getHeight()) / 2.0f);
	newMonster->add<WalkerComponent>(stats->timeToMove);
	newMonster->add<RenderableComponent>(std::move(renderable));
	newMonster->add<MonsterComponent>(stats->hp);
}

}
