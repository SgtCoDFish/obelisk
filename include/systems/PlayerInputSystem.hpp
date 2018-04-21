#ifndef OBELISK_PLAYERINPUTSYSTEM_HPP
#define OBELISK_PLAYERINPUTSYSTEM_HPP

#include <Ashley/core/EntitySystem.hpp>
#include <Ashley/core/Family.hpp>
#include <Ashley/systems/IteratingSystem.hpp>
#include <Ashley/core/ComponentMapper.hpp>

#include <components/PositionComponent.hpp>
#include <components/ClickableComponent.hpp>

namespace APG {
class SDLInputManager;

class Camera;
}

namespace obelisk {

class PlayerInputSystem final : public ashley::IteratingSystem {
public:
	explicit PlayerInputSystem(int64_t priority, APG::SDLInputManager *inputManager, APG::Camera *camera);

	void update(float deltaTime) override;

	void processEntity(ashley::Entity *entity, float deltaTime) override;

private:
	APG::SDLInputManager *inputManager;
	APG::Camera *camera;

	float mouseWorldX{0.0f};
	float mouseWorldY{0.0f};

	ashley::ComponentMapper<PositionComponent> positionMapper{ashley::ComponentMapper<PositionComponent>::getMapper()};
	ashley::ComponentMapper<ClickableComponent> clickableMapper{ashley::ComponentMapper<ClickableComponent>::getMapper()};
};

}

#endif
