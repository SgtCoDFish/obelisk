#ifndef OBELISK_PLAYERINPUTSYSTEM_HPP
#define OBELISK_PLAYERINPUTSYSTEM_HPP

#include <Ashley/core/EntitySystem.hpp>
#include <Ashley/core/Family.hpp>
#include <Ashley/systems/IteratingSystem.hpp>
#include <Ashley/core/ComponentMapper.hpp>

#include <components/PositionComponent.hpp>
#include <components/ClickableComponent.hpp>
#include <ObeliskState.hpp>
#include <components/CarryableComponent.hpp>
#include <components/TowerComponent.hpp>

namespace APG {
class SDLInputManager;

class Camera;
}

namespace obelisk {

class PlayerInputSystem final : public ashley::IteratingSystem {
public:
	explicit PlayerInputSystem(int64_t priority, APG::SDLInputManager *inputManager, APG::Camera *camera, ObeliskState *state);

	void update(float deltaTime) override;

	void processEntity(ashley::Entity *entity, float deltaTime) override;

private:
	APG::SDLInputManager *inputManager;
	APG::Camera *camera;

	ObeliskState *state;

	float mouseWorldX{0.0f};
	float mouseWorldY{0.0f};

	bool processDrop(ashley::Entity *entity, PositionComponent *position, ClickableComponent *clickable, float deltaTime);
	bool processPickup(ashley::Entity *entity, PositionComponent *position, ClickableComponent *clickable, float deltaTime);

	void processUnpickup(ashley::Entity *entity, float deltaTime);

	bool isInside(PositionComponent *position, ClickableComponent *clickable);

	ashley::ComponentMapper<PositionComponent> positionMapper{ashley::ComponentMapper<PositionComponent>::getMapper()};
	ashley::ComponentMapper<ClickableComponent> clickableMapper{ashley::ComponentMapper<ClickableComponent>::getMapper()};
	ashley::ComponentMapper<CarryableComponent> carryableMapper{ashley::ComponentMapper<CarryableComponent>::getMapper()};
	ashley::ComponentMapper<TowerComponent> towerMapper{ashley::ComponentMapper<TowerComponent>::getMapper()};
};

}

#endif
