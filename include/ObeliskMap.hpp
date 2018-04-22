#ifndef OBELISK_OBELISKMAP_HPP
#define OBELISK_OBELISKMAP_HPP

#include <memory>
#include <utility>

#include "APG/tiled/PackedTmxRenderer.hpp"

namespace obelisk {

class ObeliskMap final {
public:
	explicit ObeliskMap(std::unique_ptr<APG::PackedTmxRenderer> &&renderer);

	std::unique_ptr<APG::PackedTmxRenderer> renderer;
	glm::ivec2 traversibleStart{-1, -1};
	glm::ivec2 traversibleEnd{-1, -1};

	int getWidthInTiles() const {
		return renderer->getMap()->GetWidth();
	}

	int getHeightInTiles() const {
		return renderer->getMap()->GetHeight();
	}

	int getTileWidth() const {
		return renderer->getMap()->GetTileWidth();
	}

	int getTileHeight() const {
		return renderer->getMap()->GetTileHeight();
	}

	bool isTraversible(int x, int y) const;

private:
	void calculateTraversible();

	std::unique_ptr<bool[]> traversibleTiles{nullptr};
};

}

#endif
