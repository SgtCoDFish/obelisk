#ifndef OBELISK_TOASTSYSTEM_HPP
#define OBELISK_TOASTSYSTEM_HPP

#include <Ashley/systems/IteratingSystem.hpp>
#include <components/PositionComponent.hpp>
#include <APG/font/FontManager.hpp>
#include <Ashley/core/ComponentMapper.hpp>
#include <components/RenderableComponent.hpp>
#include <components/ToastComponent.hpp>
#include <glm/vec2.hpp>

namespace obelisk {

class ToastSystem final : public ashley::IteratingSystem {
public:
	explicit ToastSystem(int64_t priority, APG::FontManager *fontManager, APG::FontManager::font_handle font);

	void addToast(const std::string &text, glm::vec2 position);

	void addedToEngine(ashley::Engine &engine) override;

	void removedFromEngine(ashley::Engine &engine) override;

	void processEntity(ashley::Entity *entity, float deltaTime) override;

private:
	APG::FontManager *fontManager;
	APG::FontManager::font_handle font;

	ashley::Engine *engine{nullptr};

	ashley::ComponentMapper<PositionComponent> positionMapper{ashley::ComponentMapper<PositionComponent>::getMapper()};
	ashley::ComponentMapper<RenderableComponent> renderableMapper{
			ashley::ComponentMapper<RenderableComponent>::getMapper()};
	ashley::ComponentMapper<ToastComponent> toastMapper{ashley::ComponentMapper<ToastComponent>::getMapper()};
};

}

#endif
