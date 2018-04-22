#include <Ashley/Ashley.hpp>
#include <components/PositionComponent.hpp>
#include <components/RenderableComponent.hpp>
#include <components/WalkerComponent.hpp>

#include "systems/EntitySpawnSystem.hpp"

namespace obelisk {


EntitySpawnSystem::EntitySpawnSystem(int64_t priority, float interval, ObeliskState *state,
									 std::vector<APG::SpriteBase *> sprites) :
		IntervalSystem(interval, priority),
		state{state},
		sprites{std::move(sprites)} {
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
	while (sprite == nullptr || sprite == lastSprite) {
		sprite = sprites[distribution(state->rand)];
	}

	lastSprite = sprite;

	auto renderable = std::make_unique<RenderableComponent>(sprite);

	newMonster->add<PositionComponent>(renderer->getPosition().x + renderer->getPixelWidth(),
									   renderer->getPosition().y +
									   (renderer->getPixelHeight() - sprite->getHeight()) / 2.0f);
	newMonster->add<WalkerComponent>(1.0f);
	newMonster->add<RenderableComponent>(std::move(renderable));
}

}
