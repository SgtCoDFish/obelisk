#include <cstdint>
#include <cstdlib>

#include <iostream>
#include <memory>
#include <chrono>
#include <vector>
#include <numeric>
#include <systems/RenderSystem.hpp>
#include <components/TowerComponent.hpp>
#include <systems/CarrySystem.hpp>
#include <systems/DeathSystem.hpp>
#include <components/WalkerComponent.hpp>
#include <systems/WalkingSystem.hpp>

#include "APG/GL.hpp"
#include "APG/SDL.hpp"

#include "APG/SXXDL.hpp"
#include "APG/core/Game.hpp"
#include "APG/core/SDLGame.hpp"
#include "APG/graphics/Camera.hpp"

#include "obelisk.hpp"
#include "components/RenderableComponent.hpp"
#include "components/PositionComponent.hpp"
#include "systems/RenderSystem.hpp"
#include "systems/PlayerInputSystem.hpp"

INITIALIZE_EASYLOGGINGPP

namespace obelisk {
#if defined (__EMSCRIPTEN__)
#include <emscripten.h>
const char * Obelisk::vertexShaderFilename = "assets/vertex-es3.glslv";
const char * Obelisk::fragmentShaderFilename = "assets/frag-es3.glslf";
#else
const char *Obelisk::vertexShaderFilename = "assets/vertex.glslv";
const char *Obelisk::fragmentShaderFilename = "assets/frag.glslf";
#endif


bool Obelisk::init() {
	const auto logger = el::Loggers::getLogger("obelisk");

	state = std::make_shared<ObeliskState>();

	shaderProgram = APG::ShaderProgram::fromFiles(vertexShaderFilename, fragmentShaderFilename);

	camera = std::make_unique<APG::Camera>(screenWidth, screenHeight);
	constexpr const auto zoom = 2.0f;
	camera->setZoom(zoom);
	camera->setToOrtho(false, screenWidth, screenHeight);
	spriteBatch = std::make_unique<APG::SpriteBatch>(shaderProgram.get());

	tmxRenderer = std::make_unique<APG::PackedTmxRenderer>("assets/map.tmx", spriteBatch.get(), 2048, 1024);
	glm::vec2 tmxRendererPos{
			(screenWidth * 0.95f - (tmxRenderer->getPixelWidth() / zoom)) * 2.0f,
			(screenHeight - (tmxRenderer->getPixelHeight() / zoom)),
	};
	tmxRenderer->setPosition(tmxRendererPos);

	parseTraversible(logger);

	packAssets(logger);

	auto glError = glGetError();
	if (glError != GL_NO_ERROR) {
		while (glError != GL_NO_ERROR) {
			logger->fatal("Error in OpenGL while loading: ", APG::prettyGLError(glError));
			glError = glGetError();
		}

		return false;
	}

	initECS(logger);

	return true;
}

void Obelisk::packAssets(el::Logger *logger) {
	packedAssets = std::make_unique<APG::PackedTexture>(1024, 1024);

	auto maybeAnimals = packedAssets->insertFile("assets/animals.png");
	if (!maybeAnimals) {
		logger->fatal("Couldn't pack required animal assets.");
		return;
	}

	auto maybeCards = packedAssets->insertFile("assets/cards.png");
	if (!maybeCards) {
		logger->fatal("Couldn't pack required card assets.");
		return;
	}

	auto maybeSmallCards = packedAssets->insertFile("assets/cards_small.png");
	if (!maybeSmallCards) {
		logger->fatal("Couldn't pack required small card assets.");
		return;
	}

	packedAssets->commitPack();

	this->animalPackRect = *maybeAnimals;
	this->cardPackRect = *maybeCards;
	this->smallCardPackRect = *maybeSmallCards;

	animal = packedAssets->makeSpritePtr({animalPackRect.x + (64 * 3), animalPackRect.y + 64, 64, 64});

	blueCardBack = packedAssets->makeSpritePtr({cardPackRect.x + 0, cardPackRect.y + 0, 140, 190});
	redCardBack = packedAssets->makeSpritePtr({cardPackRect.x + 140, cardPackRect.y + 0, 140, 190});
	greenCardBack = packedAssets->makeSpritePtr({cardPackRect.x + 280, cardPackRect.y + 0, 140, 190});

	smallBlueCardBack = packedAssets->makeSpritePtr({smallCardPackRect.x + 0, smallCardPackRect.y + 0, 28, 38});
	smallRedCardBack = packedAssets->makeSpritePtr({smallCardPackRect.x + 28, smallCardPackRect.y + 0, 28, 38});
	smallGreenCardBack = packedAssets->makeSpritePtr({smallCardPackRect.x + 56, smallCardPackRect.y + 0, 28, 38});

	towerSprite = tmxRenderer->getPackedTexture()->makeSpritePtr({19 * 64, 7 * 64, 64, 64});
}

void Obelisk::initECS(el::Logger *logger) {
	const auto rendererPos = tmxRenderer->getPosition();

	auto testMonster = engine->addEntity();

	testMonster->add<PositionComponent>(rendererPos.x + tmxRenderer->getPixelWidth(),
										rendererPos.y + (tmxRenderer->getPixelHeight() - animal->getHeight()) / 2.0f);
	testMonster->add<RenderableComponent>(animal.get());
	testMonster->add<WalkerComponent>(5.0f);

	auto towerObjects = tmxRenderer->getObjectGroup("towers");

	for (const auto &object : towerObjects) {
		auto tower = engine->addEntity();

		auto renderable = std::make_unique<RenderableComponent>(towerSprite.get());
		renderable->visible = false;

		tower->add<PositionComponent>(object.position.x + rendererPos.x, object.position.y + rendererPos.y);
		tower->add<ClickableComponent>(SDL_Rect{0, 0, renderable->sprite->getWidth(), renderable->sprite->getHeight()});
		tower->add<TowerComponent>(object.name);
		tower->add(std::move(renderable));
	}

	logger->info("Created %v tower bases", towerObjects.size());

	auto displayedCard = engine->addEntity();
	displayedCard->add<PositionComponent>(10, 10);
	displayedCard->add<RenderableComponent>(redCardBack.get());
	displayedCard->add<ClickableComponent>(SDL_Rect{0, 0, redCardBack->getWidth(), redCardBack->getHeight()});
	displayedCard->add<CarryableComponent>(smallRedCardBack.get());

	engine->addSystem<RenderSystem>(spriteBatch.get(), 1000);
	engine->addSystem<CarrySystem>(inputManager.get(), 2500);
	engine->addSystem<WalkingSystem>(3500, state);
	playerInputSystem = engine->addSystem<PlayerInputSystem>(5000, inputManager.get(), camera.get(), state);
	engine->addSystem<DeathSystem>(1000000);
}

void Obelisk::parseTraversible(el::Logger *logger) {
	const auto widthInTiles = tmxRenderer->getMap()->GetWidth();
	const auto heightInTiles = tmxRenderer->getMap()->GetHeight();

	state->traversibleWidthInTiles = widthInTiles;
	state->traversibleHeightInTiles = heightInTiles;
	state->traversibleTiles = std::make_unique<bool[]>(widthInTiles * heightInTiles);

	auto layers = tmxRenderer->getMap()->GetTileLayers();

	for (const auto &layer : layers) {
		if (layer->GetProperties().GetBoolProperty("traversible", false)) {
			logger->info("Layer '%v' is traversible", layer->GetName());
			for (int y = 0; y < heightInTiles; ++y) {
				for (int x = 0; x < widthInTiles; ++x) {
					const auto tile = layer->GetTile(x, y);

					if (tile.id >= 1) {
						state->traversibleTiles[y * widthInTiles + x] = true;
					}
				}
			}
		}
	}

	// TODO: Don't assume there's only 1 point on the right

	const auto searchX = widthInTiles - 1;
	for (int y = 0; y < heightInTiles; ++y) {
		if (state->traversibleTiles[y * widthInTiles + searchX]) {
			state->traversibleStart = glm::ivec2{searchX, y};
			logger->info("Set move search start to %v, %v", searchX, y);
			break;
		}
	}
}

void Obelisk::render(float deltaTime) {
	glClearColor(0.18f, 0.80f, 0.44f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	if (inputManager->isKeyJustPressed(SDL_SCANCODE_SPACE)) {
		//el::Loggers::getLogger("obelisk")->info("Mouse (x, y) = (%v, %v)", );
	}

	camera->update();
	spriteBatch->setProjectionMatrix(camera->combinedMatrix);

	tmxRenderer->renderAllAndUpdate(deltaTime);

	engine->update(deltaTime);

	SDL_GL_SwapWindow(window.get());
}

Obelisk::Obelisk(const std::string &windowTitle, uint32_t windowWidth, uint32_t windowHeight, uint32_t glContextMajor,
				 uint32_t glContextMinor, uint32_t windowX, uint32_t windowY) :
		SDLGame(windowTitle, windowWidth, windowHeight, glContextMajor, glContextMinor, windowX, windowY),
		engine{std::make_unique<ashley::Engine>()} {
}

}

struct loop_arg {
	obelisk::Obelisk *game{nullptr};
	el::Logger *logger{nullptr};
	bool done{false};
	std::chrono::time_point<std::chrono::high_resolution_clock> timepoint;
	std::vector<float> timesTaken;
};

void loop(void *v_arg) {
	auto arg = static_cast<loop_arg *>(v_arg);

	auto timeNow = std::chrono::high_resolution_clock::now();
	float deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - arg->timepoint).count() / 1000.0f;

	arg->timepoint = timeNow;
	arg->timesTaken.push_back(deltaTime);

	arg->done = arg->game->update(deltaTime);

	if (arg->timesTaken.size() >= 500) {
		const float sum = std::accumulate(arg->timesTaken.begin(), arg->timesTaken.end(), 0.0f);
		const float fps = 1 / (sum / arg->timesTaken.size());

		arg->logger->info("FPS: %v", fps);

		arg->timesTaken.clear();
	}
}

int main(int argc, char *argv[]) {
	START_EASYLOGGINGPP(argc, argv);

	const std::string windowTitle("obelisk");
	const uint32_t windowWidth = 800;
	const uint32_t windowHeight = 480;

	APG::Game::setLoggerToAPGStyle("obelisk");
	const auto logger = el::Loggers::getLogger("obelisk");
	auto game = std::make_unique<obelisk::Obelisk>(windowTitle, windowWidth, windowHeight);

	if (!game->init()) {
		return EXIT_FAILURE;
	}

	loop_arg arg;
	arg.game = game.get();
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

