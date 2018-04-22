#include "ObeliskMap.hpp"

namespace obelisk {

ObeliskMap::ObeliskMap(std::unique_ptr <APG::PackedTmxRenderer> &&renderer) :
		renderer{std::move(renderer)} {
	calculateTraversible();
}

void ObeliskMap::calculateTraversible() {
	auto logger = el::Loggers::getLogger("obelisk");
	const auto widthInTiles = getWidthInTiles();
	const auto heightInTiles = getHeightInTiles();

	traversibleTiles = std::make_unique<bool[]>(widthInTiles * heightInTiles);

	auto layers = renderer->getMap()->GetTileLayers();

	for (const auto &layer : layers) {
		if (layer->GetProperties().GetBoolProperty("traversible", false)) {
			logger->info("Layer '%v' is traversible", layer->GetName());
			for (int y = 0; y < heightInTiles; ++y) {
				for (int x = 0; x < widthInTiles; ++x) {
					const auto tile = layer->GetTile(x, y);

					if (tile.id >= 1) {
						traversibleTiles[y * widthInTiles + x] = true;
					}
				}
			}
		}
	}

	// TODO: Don't assume there's only 1 point on the right
	const auto startSearchX = widthInTiles - 1;
	for (int y = 0; y < heightInTiles; ++y) {
		if (isTraversible(startSearchX, y)) {
			traversibleStart = glm::ivec2{startSearchX, y};
			logger->info("Set move search start to %v, %v", startSearchX, y);
			break;
		}
	}

	const auto endSearchX = 0;
	for (int y = 0; y < heightInTiles; ++y) {
		if (isTraversible(endSearchX, y)) {
			traversibleEnd = glm::ivec2{endSearchX, y};
			logger->info("Set move search end to %v, %v", endSearchX, y);
			break;
		}
	}
}

bool ObeliskMap::isTraversible(int x, int y) const {
	if (x < 0 || x >= getWidthInTiles() || y < 0 || y >= getHeightInTiles()) {
		//el::Loggers::getLogger("obelisk")->error("Bad coords passed to isTraversible: %v, %v", x, y);
		return false;
	}

	return traversibleTiles[y * getWidthInTiles() + x];
}

}
