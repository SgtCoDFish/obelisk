#ifndef OBELISK_RENDERSYSTEM_HPP
#define OBELISK_RENDERSYSTEM_HPP

#include <cstdint>

#include <Ashley/systems/IteratingSystem.hpp>
#include <Ashley/core/ComponentMapper.hpp>
#include <components/AnnotationComponent.hpp>

#include "components/ColorComponent.hpp"
#include "components/PositionComponent.hpp"
#include "components/RenderableComponent.hpp"

namespace APG {
class SpriteBatch;
}

namespace obelisk {

class RenderSystem final : public ashley::IteratingSystem {
public:
	explicit RenderSystem(APG::SpriteBatch *batch, int64_t priority);

	void update(float deltaTime) override;

	void processEntity(ashley::Entity *entity, float deltaTime) override;

private:
	APG::SpriteBatch *batch;

	ashley::ComponentMapper<PositionComponent> positionMapper = ashley::ComponentMapper<PositionComponent>::getMapper();
	ashley::ComponentMapper<RenderableComponent> renderableMapper = ashley::ComponentMapper<RenderableComponent>::getMapper();
	ashley::ComponentMapper<ColorComponent> colorMapper = ashley::ComponentMapper<ColorComponent>::getMapper();
	ashley::ComponentMapper<AnnotationComponent> annotationMapper = ashley::ComponentMapper<AnnotationComponent>::getMapper();
};

}

#endif
