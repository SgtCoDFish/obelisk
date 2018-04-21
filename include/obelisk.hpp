#ifndef INCLUDE_TEST_APGGLRENDERTEST_HPP_
#define INCLUDE_TEST_APGGLRENDERTEST_HPP_

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

namespace APG {

class APGGLRenderTest final : public APG::SDLGame {
public:
	explicit APGGLRenderTest(const std::string &windowTitle, uint32_t windowWidth, uint32_t windowHeight,
	        uint32_t glContextMajor = 3, uint32_t glContextMinor = 2, uint32_t windowX = SDL_WINDOWPOS_CENTERED,
	        uint32_t windowY = SDL_WINDOWPOS_CENTERED) :
			        SDLGame(windowTitle, windowWidth, windowHeight, glContextMajor, glContextMinor, windowX, windowY) {
	}

	virtual ~APGGLRenderTest() = default;

	bool init() override;
	void render(float deltaTime) override;

private:
	static const char *vertexShaderFilename;
	static const char *fragmentShaderFilename;

	std::unique_ptr<ShaderProgram> shaderProgram;

	std::unique_ptr<Camera> camera;
	std::unique_ptr<SpriteBatch> spriteBatch;

	std::unique_ptr<PackedTmxRenderer> rendererOne;
	std::unique_ptr<PackedTmxRenderer> rendererTwo;
	PackedTmxRenderer *currentRenderer = nullptr;

	std::unique_ptr<Texture> playerTexture;
	std::vector<Sprite> playerFrames;
	std::unique_ptr<AnimatedSprite> playerAnimation;

	std::unique_ptr<Texture> miniTexture;
	std::unique_ptr<Sprite> miniPlayer;

	SpriteBase * currentPlayer = nullptr;

	std::unique_ptr<Sprite> bigSprite;

	FontManager::font_handle font = -1;

	SpriteBase *fontSprite = nullptr;
};

}

#endif /* APGGLRENDERTEST_HPP_ */
