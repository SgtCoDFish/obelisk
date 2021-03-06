#ifndef OBELISK_PLAYERINPUTSYSTEM_HPP
#define OBELISK_PLAYERINPUTSYSTEM_HPP

#include <Ashley/core/EntitySystem.hpp>
#include <Ashley/core/Family.hpp>
#include <Ashley/systems/IteratingSystem.hpp>
#include <Ashley/core/ComponentMapper.hpp>

#include <ObeliskState.hpp>
#include <components/PositionComponent.hpp>
#include <components/ClickableComponent.hpp>
#include <components/CarryableComponent.hpp>
#include <components/TowerComponent.hpp>
#include <components/TrashComponent.hpp>
#include <components/TowerUpgradeComponent.hpp>
#include <components/DeckComponent.hpp>

namespace APG {
class SDLInputManager;

class Camera;
}

namespace obelisk {

class PlayerInputSystem final : public ashley::IteratingSystem {
public:
	explicit PlayerInputSystem(int64_t priority, APG::SDLInputManager *inputManager, APG::Camera *camera, std::shared_ptr<ObeliskState> state);

	void update(float deltaTime) override;

	void processEntity(ashley::Entity *entity, float deltaTime) override;

	void addedToEngine(ashley::Engine &engine) override;
	void removedFromEngine(ashley::Engine &engine) override;

private:
	ashley::Engine *engine{nullptr};
	APG::SDLInputManager *inputManager;
	APG::Camera *camera;

	std::shared_ptr<ObeliskState> state;

	float mouseWorldX{0.0f};
	float mouseWorldY{0.0f};

	bool processDrop(ashley::Entity *entity, PositionComponent *position, ClickableComponent *clickable, float deltaTime);
	bool processPickup(ashley::Entity *entity, PositionComponent *position, ClickableComponent *clickable, float deltaTime);

	void processUnpickup(ashley::Entity *entity, float deltaTime);

	bool isInside(PositionComponent *position, ClickableComponent *clickable);

	ashley::ComponentMapper<PositionComponent> positionMapper{ashley::ComponentMapper<PositionComponent>::getMapper()};
	ashley::ComponentMapper<ClickableComponent> clickableMapper{ashley::ComponentMapper<ClickableComponent>::getMapper()};
	ashley::ComponentMapper<CarryableComponent> carryableMapper{ashley::ComponentMapper<CarryableComponent>::getMapper()};
	ashley::ComponentMapper<DeckComponent> deckMapper{ashley::ComponentMapper<DeckComponent>::getMapper()};
	ashley::ComponentMapper<TowerComponent> towerMapper{ashley::ComponentMapper<TowerComponent>::getMapper()};
	ashley::ComponentMapper<TowerUpgradeComponent> towerUpgradeMapper{ashley::ComponentMapper<TowerUpgradeComponent>::getMapper()};
	ashley::ComponentMapper<TrashComponent> trashMapper{ashley::ComponentMapper<TrashComponent>::getMapper()};
};

}

#endif
