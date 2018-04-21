#ifndef INCLUDE_OBELISK_HPP_
#define INCLUDE_OBELISK_HPP_

#include <string>
#include <memory>
#include <utility>

#include "APG/SDL.hpp"

#include "Tmx.h"

#include "APG/SXXDL.hpp"
#include "APG/core/Game.hpp"
#include "APG/core/SDLGame.hpp"
#include "APG/core/APGCommon.hpp"
#include "APG/graphics/ShaderProgram.hpp"
#include "APG/graphics/Buffer.hpp"
#include "APG/graphics/VAO.hpp"
#include "APG/graphics/Texture.hpp"
#include "APG/graphics/SpriteBatch.hpp"
#include "APG/graphics/Sprite.hpp"
#include "APG/graphics/Camera.hpp"
#include "APG/tiled/GLTmxRenderer.hpp"
#include "APG/tiled/PackedTmxRenderer.hpp"

#include "Ashley/Ashley.hpp"

namespace obelisk {

class Obelisk final : public APG::SDLGame {
public:
	explicit Obelisk(const std::string &windowTitle, uint32_t windowWidth, uint32_t windowHeight,
	        uint32_t glContextMajor = 3, uint32_t glContextMinor = 2, uint32_t windowX = SDL_WINDOWPOS_CENTERED,
	        uint32_t windowY = SDL_WINDOWPOS_CENTERED);

	~Obelisk() override = default;

	bool init() override;
	void render(float deltaTime) override;

private:
	static const char *vertexShaderFilename;
	static const char *fragmentShaderFilename;

	void initECS(el::Logger * logger);
	void packAssets(el::Logger * logger);

	std::unique_ptr<ashley::Engine> engine;

	std::unique_ptr<APG::ShaderProgram> shaderProgram;

	std::unique_ptr<APG::Camera> camera;
	std::unique_ptr<APG::SpriteBatch> spriteBatch;

	std::unique_ptr<APG::PackedTexture> packedAssets;
	SDL_Rect animalPackRect; // the packed location of the animal asset file
	SDL_Rect cardPackRect; // the packed location of the card asset file

	std::unique_ptr<APG::Sprite> animal;
	std::unique_ptr<APG::Sprite> blueCardBack;
	std::unique_ptr<APG::Sprite> redCardBack;
	std::unique_ptr<APG::Sprite> greenCardBack;
	std::unique_ptr<APG::Sprite> towerSprite;

	std::unique_ptr<APG::PackedTmxRenderer> tmxRenderer;
};

}

#endif
