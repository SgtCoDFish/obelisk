#ifndef OBELISK_OBELISKSTATE_HPP
#define OBELISK_OBELISKSTATE_HPP

#include <Ashley/core/Entity.hpp>

#include <glm/vec2.hpp>
#include "ObeliskMap.hpp"

namespace obelisk {

struct ObeliskState {
	ashley::Entity *heldItem{nullptr};
	ObeliskMap *currentMap{nullptr};
};

}

#endif
