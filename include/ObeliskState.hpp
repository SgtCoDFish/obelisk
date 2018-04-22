#ifndef OBELISK_OBELISKSTATE_HPP
#define OBELISK_OBELISKSTATE_HPP

#include <Ashley/core/Entity.hpp>

#include <glm/vec2.hpp>

namespace obelisk {

struct ObeliskState {
	ashley::Entity *heldItem{nullptr};

	std::unique_ptr<bool[]> traversibleTiles{nullptr};
	int traversibleWidthInTiles{0};
	int traversibleHeightInTiles{0};
	glm::ivec2 traversibleStart{0, 0};
};

}

#endif
