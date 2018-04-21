#ifndef OBELISK_RENDERSYSTEM_HPP
#define OBELISK_RENDERSYSTEM_HPP

#include <cstdint>

#include <Ashley/systems/IteratingSystem.hpp>

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
};

}

#endif
