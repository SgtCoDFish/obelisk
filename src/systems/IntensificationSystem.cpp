#include <components/IntensificationComponent.hpp>
#include <Ashley/core/ComponentMapper.hpp>
#include "systems/IntensificationSystem.hpp"
#include "systems/ToastSystem.hpp"
#include "systems/EntitySpawnSystem.hpp"
#include "Ashley/core/Family.hpp"
#include <glm/vec2.hpp>
#include <components/DeathComponent.hpp>
#include <components/DeckComponent.hpp>
#include <components/CardComponent.hpp>
#include <components/CarryableComponent.hpp>
#include <components/ClickableComponent.hpp>

namespace obelisk {

IntensificationSystem::IntensificationSystem(int64_t priority, std::shared_ptr<ObeliskState> state,
											 APG::SpriteBase *speedUpgrade, APG::SpriteBase *levelUpgrade,
											 APG::SpriteBase *cardFront, APG::SpriteBase *smallCardFront) :
		IteratingSystem(ashley::Family::getFor({typeid(IntensificationComponent)}), priority),
		state{state},
		speedUpgrade{speedUpgrade},
		levelUpgrade{levelUpgrade},
		cardFront{cardFront},
		smallCardFront{smallCardFront} {
}

void IntensificationSystem::processEntity(ashley::Entity *entity, float deltaTime) {
	auto comp = ashley::ComponentMapper<IntensificationComponent>::getMapper().get(entity);
	const auto textX = (state->screenWidth - (80 / 2));
	const auto textY = (state->screenHeight / 2.0f * 2.0f);
	if (comp->level % 6 == 0) {
		state->toastSystem->addBigToast("SPEED UP!", glm::vec2{textX, textY});
		auto currentInterval = state->entitySpawnSystem->getInterval();
		float diff = 1.0f;
		if (currentInterval <= 5.0f) {
			diff = 0.5f;
		}
		state->entitySpawnSystem->setInterval(std::max(currentInterval - diff, 1.5f));
	} else if (comp->level % 3 == 0) {
		state->toastSystem->addBigToast("UPGRADE ADDED", glm::vec2{textX, textY});
		auto upgrade = std::make_unique<ashley::Entity>();

		if (state->rand() % 2 == 0) {
			upgrade->add<PositionComponent>(10, 10);
			upgrade->add<RenderableComponent>(cardFront, speedUpgrade);
			upgrade->add<ClickableComponent>(
					SDL_Rect{0, 0, cardFront->getWidth(), cardFront->getHeight()});
			upgrade->add<CarryableComponent>(smallCardFront, UpgradeType::SPEED,
											 speedUpgrade);
			upgrade->add<CardComponent>();
		} else {
			upgrade->add<PositionComponent>(10, 10);
			upgrade->add<RenderableComponent>(cardFront, levelUpgrade);
			upgrade->add<ClickableComponent>(
					SDL_Rect{0, 0, cardFront->getWidth(), cardFront->getHeight()});
			upgrade->add<CarryableComponent>(smallCardFront, UpgradeType::LEVEL, levelUpgrade);
			upgrade->add<CardComponent>();
		}
		state->upgradeDeck->getComponent<DeckComponent>()->cards.emplace_front(std::move(upgrade));
	}

	entity->add<DeathComponent>();
}

}