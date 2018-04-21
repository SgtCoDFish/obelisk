#include <cstdint>
#include <cstdlib>

#include <iostream>
#include <memory>
#include <chrono>
#include <vector>
#include <numeric>

#include "APG/GL.hpp"
#include "APG/SDL.hpp"

#include "APG/SXXDL.hpp"
#include "APG/core/Game.hpp"
#include "APG/core/SDLGame.hpp"
#include "APG/graphics/Camera.hpp"

#include "test/APGGLRenderTest.hpp"

INITIALIZE_EASYLOGGINGPP

#if defined (__EMSCRIPTEN__)
#include <emscripten.h>
const char * APG::APGGLRenderTest::vertexShaderFilename = "assets/pass_vertex-es3.glslv";
const char * APG::APGGLRenderTest::fragmentShaderFilename = "assets/red_frag-es3.glslf";
#else
const char *APG::APGGLRenderTest::vertexShaderFilename = "assets/pass_vertex.glslv";
const char *APG::APGGLRenderTest::fragmentShaderFilename = "assets/red_frag.glslf";
#endif

bool APG::APGGLRenderTest::init() {
	const auto logger = el::Loggers::getLogger("APG");

	auto map1 = std::make_unique<Tmx::Map>();
	map1->ParseFile("assets/sample_indoor.tmx");

	if (map1->HasError()) {
		logger->fatal("Error loading map1: %v", map1->GetErrorText());
		return false;
	}

	auto map2 = std::make_unique<Tmx::Map>();
	map2->ParseFile("assets/world1.tmx");

	if (map2->HasError()) {
		logger->fatal("Error loading map2: %v", map2->GetErrorText());
		return false;
	}

	shaderProgram = ShaderProgram::fromFiles(vertexShaderFilename, fragmentShaderFilename);

	camera = std::make_unique<Camera>(screenWidth, screenHeight);
	camera->setToOrtho(false, screenWidth, screenHeight);
	spriteBatch = std::make_unique<SpriteBatch>(shaderProgram.get());

	rendererOne = std::make_unique<PackedTmxRenderer>("assets/sample_indoor.tmx", spriteBatch.get(), 1024, 1024);
	rendererTwo = std::make_unique<PackedTmxRenderer>("assets/world1.tmx", spriteBatch.get(), 1024, 1024);
	currentRenderer = rendererOne.get();

	playerTexture = std::make_unique<Texture>("assets/player.png");
	playerFrames = AnimatedSprite::splitTexture(playerTexture, 32, 32, 0, 32, 4);
	playerAnimation = std::make_unique<AnimatedSprite>(0.3f, playerFrames, AnimationMode::LOOP);

	miniTexture = std::make_unique<Texture>("assets/player16.png");
	miniPlayer = std::make_unique<Sprite>(miniTexture);

	currentPlayer = miniPlayer.get();

	font = fontManager->loadFontFile("assets/test_font.ttf", 12);
	const auto renderedFontSize = fontManager->estimateSizeOf(font, "Hello, World!");

	logger->info("Estimated font size: (w, h) = (%v, %v).", renderedFontSize.x, renderedFontSize.y);

	fontSprite = fontManager->renderText(font, "Hello, world!", true, FontRenderMethod::NICE);

	auto glError = glGetError();
	if (glError != GL_NO_ERROR) {
		while (glError != GL_NO_ERROR) {
			logger->fatal("Error in OpenGL while loading: ", prettyGLError(glError));
			glError = glGetError();
		}

		return false;
	}

	return true;
}

void APG::APGGLRenderTest::render(float deltaTime) {
	glClearColor(0.313725f, 0.674510f, 0.239216f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	static float playerX = 128.0f, playerY = 128.0f;
	const glm::vec3 textScreenPosition{50.0f, screenHeight - 50.0f, 0.0f};
	static glm::vec3 textPos;

	// will move the player quickly
	if (inputManager->isKeyPressed(SDL_SCANCODE_UP)) {
		playerY -= currentPlayer->getHeight();
	} else if (inputManager->isKeyPressed(SDL_SCANCODE_DOWN)) {
		playerY += currentPlayer->getHeight();
	} else if (inputManager->isKeyPressed(SDL_SCANCODE_LEFT)) {
		playerX -= currentPlayer->getWidth();
	} else if (inputManager->isKeyPressed(SDL_SCANCODE_RIGHT)) {
		playerX += currentPlayer->getWidth();
	}

	// will move the player once per press
	if (inputManager->isKeyJustPressed(SDL_SCANCODE_W)) {
		playerY -= currentPlayer->getHeight();
	} else if (inputManager->isKeyJustPressed(SDL_SCANCODE_S)) {
		playerY += currentPlayer->getHeight();
	} else if (inputManager->isKeyJustPressed(SDL_SCANCODE_A)) {
		playerX -= currentPlayer->getWidth();
	} else if (inputManager->isKeyJustPressed(SDL_SCANCODE_D)) {
		playerX += currentPlayer->getWidth();
	}

	if (inputManager->isKeyJustPressed(SDL_SCANCODE_SPACE)) {
		if (currentRenderer == rendererOne.get()) {
			currentRenderer = rendererTwo.get();
			currentPlayer = playerAnimation.get();
		} else {
			currentRenderer = rendererOne.get();
			currentPlayer = miniPlayer.get();
			playerX = 18 * 16;
			playerY = 21 * 16;
		}
	}

	camera->position.x = playerX - screenWidth / 2.0f;
	camera->position.y = playerY - screenHeight / 2.0f;

	camera->update();
	spriteBatch->setProjectionMatrix(camera->combinedMatrix);

	currentRenderer->renderAllAndUpdate(deltaTime);
	playerAnimation->update(deltaTime);

	textPos = camera->unproject(textScreenPosition);

	spriteBatch->begin();
	spriteBatch->draw(currentPlayer, playerX, playerY);
	// draw a weird quarter version of the player to test out extended draw method
	//spriteBatch->draw(currentPlayer->getTexture(), playerX, playerY, currentPlayer->getWidth() * 2,
	//		currentPlayer->getHeight() * 2, 0.0f, 0.0f, currentPlayer->getWidth() * 0.5f,
	//		currentPlayer->getHeight() * 0.5f);
	spriteBatch->draw(fontSprite, textPos.x, textPos.y);
	spriteBatch->end();

	SDL_GL_SwapWindow(window.get());
}

struct loop_arg {
	APG::APGGLRenderTest *rpg { nullptr };
	el::Logger *logger { nullptr };
	bool done { false };
	std::chrono::time_point<std::chrono::high_resolution_clock> timepoint;
	std::vector<float> timesTaken;
};

void loop(void *v_arg) {
	auto arg = static_cast<loop_arg *>(v_arg);

	auto timeNow = std::chrono::high_resolution_clock::now();
	float deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - arg->timepoint).count() / 1000.0f;

	arg->timepoint = timeNow;
	arg->timesTaken.push_back(deltaTime);

	arg->done = arg->rpg->update(deltaTime);

	if (arg->timesTaken.size() >= 1000) {
		const float sum = std::accumulate(arg->timesTaken.begin(), arg->timesTaken.end(), 0.0f);
		const float fps = 1 / (sum / arg->timesTaken.size());

		arg->logger->info("FPS: ", fps);

		arg->timesTaken.clear();
	}
}

int main(int argc, char *argv[]) {
	START_EASYLOGGINGPP(argc, argv);

	const std::string windowTitle("APG GLTmxRenderer Example");
	const uint32_t windowWidth = 1280;
	const uint32_t windowHeight = 720;

	const auto logger = el::Loggers::getLogger("APG");
	auto game = std::make_unique<APG::APGGLRenderTest>(windowTitle, windowWidth, windowHeight);

	if (!game->init()) {
		return EXIT_FAILURE;
	}

	loop_arg arg;
	arg.rpg = game.get();
	arg.timepoint = std::chrono::high_resolution_clock::now();
	arg.done = false;
	arg.logger = logger;

#if defined(__EMSCRIPTEN__)
	emscripten_set_main_loop_arg(loop, &arg, 0, 1);
#else
	while (!arg.done) {
		loop(&arg);
	}
#endif

	return EXIT_SUCCESS;
}
