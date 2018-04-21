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

namespace obelisk {

class Obelisk final : public APG::SDLGame {
public:
	explicit Obelisk(const std::string &windowTitle, uint32_t windowWidth, uint32_t windowHeight,
	        uint32_t glContextMajor = 3, uint32_t glContextMinor = 2, uint32_t windowX = SDL_WINDOWPOS_CENTERED,
	        uint32_t windowY = SDL_WINDOWPOS_CENTERED) :
			        SDLGame(windowTitle, windowWidth, windowHeight, glContextMajor, glContextMinor, windowX, windowY) {
	}

	~Obelisk() override = default;

	bool init() override;
	void render(float deltaTime) override;

private:
	static const char *vertexShaderFilename;
	static const char *fragmentShaderFilename;

	float cameraX = 0.0f;
	float cameraY = 0.0f;

	std::unique_ptr<APG::ShaderProgram> shaderProgram;

	std::unique_ptr<APG::Camera> camera;
	std::unique_ptr<APG::SpriteBatch> spriteBatch;

	std::unique_ptr<APG::PackedTmxRenderer> tmxRenderer;
};

}

#endif
