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

#include "obelisk.hpp"
#include "components/RenderableComponent.hpp"
#include "components/PositionComponent.hpp"
#include <components/TowerComponent.hpp>
#include <components/WalkerComponent.hpp>
#include "systems/RenderSystem.hpp"
#include "systems/PlayerInputSystem.hpp"
#include <systems/WalkingSystem.hpp>
#include <systems/EntitySpawnSystem.hpp>
#include <systems/CarrySystem.hpp>
#include <systems/DeathSystem.hpp>
#include <systems/TowerUpgradeSystem.hpp>
#include <systems/TowerAttackSystem.hpp>
#include <components/TrashComponent.hpp>
#include <components/DeckComponent.hpp>
#include <systems/ToastSystem.hpp>
#include <systems/MovementSystem.hpp>
#include <components/CarriedComponent.hpp>
#include <components/CardComponent.hpp>

INITIALIZE_EASYLOGGINGPP
#if defined (__EMSCRIPTEN__)
#include <emscripten.h>
#endif

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

	{
		auto tmxRenderer = std::make_unique<APG::PackedTmxRenderer>("assets/map.tmx", spriteBatch.get(), 2048, 1024);
		glm::vec2 tmxRendererPos{
				(screenWidth * 0.95f - (tmxRenderer->getPixelWidth() / zoom)) * 2.0f,
				(screenHeight * 2 - (tmxRenderer->getPixelHeight())),
		};
		tmxRenderer->setPosition(tmxRendererPos);

		map = std::make_unique<ObeliskMap>(std::move(tmxRenderer));
	}

	font = fontManager->loadFontFile("assets/pixel_square.ttf", 16);

	state->currentMap = map.get();

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
		logger->fatal("Couldn't pack required giraffe assets.");
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

	auto maybeTrash = packedAssets->insertFile("assets/trashcan.png");
	if (!maybeTrash) {
		logger->fatal("Couldn't pack required trashcan assets.");
		return;
	}

	packedAssets->commitPack();

	this->animalPackRect = *maybeAnimals;
	this->cardPackRect = *maybeCards;
	this->smallCardPackRect = *maybeSmallCards;
	this->trashPackRect = *maybeTrash;

	trashSprite = packedAssets->makeSpritePtr({trashPackRect.x, trashPackRect.y + 1, 100, 100});

	monkey = packedAssets->makeSpritePtr({animalPackRect.x + (64 * 0), animalPackRect.y + (64 * 0), 64, 64});
	rabbit = packedAssets->makeSpritePtr({animalPackRect.x + (64 * 1), animalPackRect.y + (64 * 0), 64, 64});
	snake = packedAssets->makeSpritePtr({animalPackRect.x + (64 * 0), animalPackRect.y + (64 * 3), 64, 64});
	pig = packedAssets->makeSpritePtr({animalPackRect.x + (64 * 1), animalPackRect.y + (64 * 3), 64, 64});
	giraffe = packedAssets->makeSpritePtr({animalPackRect.x + (64 * 3), animalPackRect.y + (64 * 1), 64, 64});

	blueCardFront = packedAssets->makeSpritePtr({cardPackRect.x + 0, cardPackRect.y + 190, 140, 190});
	redCardFront = packedAssets->makeSpritePtr({cardPackRect.x + 140, cardPackRect.y + 190, 140, 190});
	greenCardFront = packedAssets->makeSpritePtr({cardPackRect.x + 280, cardPackRect.y + 190, 140, 190});

	blueCardBack = packedAssets->makeSpritePtr({cardPackRect.x + 0, cardPackRect.y + 0, 140, 190});
	redCardBack = packedAssets->makeSpritePtr({cardPackRect.x + 140, cardPackRect.y + 0, 140, 190});
	greenCardBack = packedAssets->makeSpritePtr({cardPackRect.x + 280, cardPackRect.y + 0, 140, 190});

	smallBlueCardBack = packedAssets->makeSpritePtr({smallCardPackRect.x + 0, smallCardPackRect.y + 0, 28, 38});
	smallRedCardBack = packedAssets->makeSpritePtr({smallCardPackRect.x + 28, smallCardPackRect.y + 0, 28, 38});
	smallGreenCardBack = packedAssets->makeSpritePtr({smallCardPackRect.x + 56, smallCardPackRect.y + 0, 28, 38});

	constructionOverlay = map->renderer->getPackedTexture()->makeSpritePtr({16 * 64, 0 * 64, 64, 64});

	towerSprite = map->renderer->getPackedTexture()->makeSpritePtr({19 * 64, 7 * 64, 64, 64});
	gunUpgradeSprite = map->renderer->getPackedTexture()->makeSpritePtr({19 * 64, 10 * 64, 64, 64});
	rocketUpgradeSprite = map->renderer->getPackedTexture()->makeSpritePtr({20 * 64, 8 * 64, 64, 64});
	plusUpgradeSprite = map->renderer->getPackedTexture()->makeSpritePtr({13 * 64, 12 * 64 + 1, 64, 64});
	gunAttackSprite = map->renderer->getPackedTexture()->makeSpritePtr({20 * 64, 11 * 64 + 1, 64, 64});
	rocketAttackSprite = map->renderer->getPackedTexture()->makeSpritePtr({21 * 64, 10 * 64 + 1, 64, 64});
}

void Obelisk::initECS(el::Logger *logger) {
	const auto rendererPos = map->renderer->getPosition();

	auto towerObjects = map->renderer->getObjectGroup("towers");

	for (const auto &object : towerObjects) {
		auto tower = engine->addEntity();

		auto renderable = std::make_unique<RenderableComponent>(towerSprite.get());

		tower->add<PositionComponent>(object.position.x + rendererPos.x, object.position.y + rendererPos.y);
		logger->info("Created tower at: %v, %v", object.position.x + rendererPos.x, object.position.y + rendererPos.y);
		tower->add<ClickableComponent>(SDL_Rect{0, 0, renderable->sprite->getWidth(), renderable->sprite->getHeight()});
		tower->add<TowerComponent>(object.name);
		tower->add(std::move(renderable));
	}

	logger->info("Created %v tower bases", towerObjects.size());

	const auto cardGapX = 5;
	const auto cardGapY = 5;

	auto displayedCard = engine->addEntity();
	displayedCard->add<PositionComponent>(cardGapX, cardGapY);
	displayedCard->add<RenderableComponent>(redCardFront.get(), gunUpgradeSprite.get());
	displayedCard->add<ClickableComponent>(SDL_Rect{0, 0, redCardFront->getWidth(), redCardFront->getHeight()});
	displayedCard->add<CarryableComponent>(smallRedCardBack.get(), UpgradeType::TOWER_GUN_UPGRADE,
										   gunUpgradeSprite.get());
	displayedCard->add<CardComponent>();

	auto displayedCard2 = engine->addEntity();
	displayedCard2->add<PositionComponent>(cardGapX * 2 + blueCardFront->getWidth(), cardGapY);
	displayedCard2->add<RenderableComponent>(blueCardFront.get(), rocketUpgradeSprite.get());
	displayedCard2->add<ClickableComponent>(SDL_Rect{0, 0, redCardFront->getWidth(), redCardFront->getHeight()});
	displayedCard2->add<CarryableComponent>(smallBlueCardBack.get(), UpgradeType::TOWER_ROCKET_UPGRADE,
											rocketUpgradeSprite.get());
	displayedCard2->add<CardComponent>();

	state->hand.push_back(displayedCard);
	state->hand.push_back(displayedCard2);

	{
		auto displayedCard3 = std::make_unique<ashley::Entity>();
		displayedCard3->add<PositionComponent>(cardGapX * 3 + greenCardFront->getWidth() * 2, cardGapY);
		displayedCard3->add<RenderableComponent>(greenCardFront.get(), plusUpgradeSprite.get());
		displayedCard3->add<ClickableComponent>(
				SDL_Rect{0, 0, greenCardFront->getWidth(), greenCardFront->getHeight()});
		displayedCard3->add<CarryableComponent>(smallGreenCardBack.get(), UpgradeType::LEVEL,
												plusUpgradeSprite.get());
		displayedCard3->add<CardComponent>();

		auto displayedCard4 = std::make_unique<ashley::Entity>();
		displayedCard4->add<PositionComponent>(cardGapX * 3 + greenCardFront->getWidth() * 2, cardGapY);
		displayedCard4->add<RenderableComponent>(greenCardFront.get(), plusUpgradeSprite.get());
		displayedCard4->add<ClickableComponent>(
				SDL_Rect{0, 0, greenCardFront->getWidth(), greenCardFront->getHeight()});
		displayedCard4->add<CarryableComponent>(smallGreenCardBack.get(), UpgradeType::LEVEL,
												plusUpgradeSprite.get());
		displayedCard4->add<CardComponent>();

		auto deck1 = engine->addEntity();
		deck1->add<PositionComponent>(cardGapX, cardGapY * 2 + blueCardBack->getHeight());
		deck1->add<RenderableComponent>(blueCardBack.get());
		deck1->add<DeckComponent>();
		deck1->add<ClickableComponent>(SDL_Rect{0, 0, blueCardBack->getWidth(), blueCardBack->getHeight()});

		deck1->getComponent<DeckComponent>()->cards.emplace_front(std::move(displayedCard3));
		deck1->getComponent<DeckComponent>()->cards.emplace_front(std::move(displayedCard4));
	}

	{
		auto trashEntity = engine->addEntity();
		auto trashRenderable = std::make_unique<RenderableComponent>(trashSprite.get());
		trashEntity->add<PositionComponent>((5), (screenHeight * 2) - trashSprite->getHeight() - 5);
		trashEntity->add<TrashComponent>();
		trashEntity->add<ClickableComponent>(SDL_Rect{0, 0, trashSprite->getWidth(), trashSprite->getHeight()});
		trashEntity->add(std::move(trashRenderable));
	}

	engine->addSystem<EntitySpawnSystem>(
			500, 10.0f, state.get(),
			std::vector<APG::SpriteBase *>(
					{giraffe.get(), monkey.get(), rabbit.get(), snake.get(), pig.get()}
			),
			std::vector<MonsterStats>({MonsterStats{2.0f}, MonsterStats{0.75f}, MonsterStats{0.5f}, MonsterStats{1.0f},
									   MonsterStats{2.5f}})
	);
	engine->addSystem<RenderSystem>(spriteBatch.get(), 1000);
	engine->addSystem<CarrySystem>(inputManager.get(), 2500);
	engine->addSystem<WalkingSystem>(3500, state);
	playerInputSystem = engine->addSystem<PlayerInputSystem>(5000, inputManager.get(), camera.get(), state);
	engine->addSystem<TowerUpgradeSystem>(10000, state, gunUpgradeSprite.get(), rocketUpgradeSprite.get(),
										  constructionOverlay.get());
	engine->addSystem<TowerAttackSystem>(15000, gunAttackSprite.get(), rocketAttackSprite.get());
	engine->addSystem<MovementSystem>(17500);
	state->toastSystem = engine->addSystem<ToastSystem>(20000, fontManager.get(), font);
	engine->addSystem<DeathSystem>(1000000, state);
}

void Obelisk::render(float deltaTime) {
	glClearColor(0.18f, 0.80f, 0.44f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	auto positionMapper = ashley::ComponentMapper<PositionComponent>::getMapper();

	for (int i = 0; i < (int) state->hand.size(); ++i) {
		auto card = state->hand[i];
		if (card->hasComponent<CarriedComponent>()) {
			continue;
		}

		auto pos = positionMapper.get(card);
		pos->position.x = 5 * (i + 1) + redCardBack->getWidth() * i;

	}

	camera->update();
	spriteBatch->setProjectionMatrix(camera->combinedMatrix);

	map->renderer->renderAllAndUpdate(deltaTime);

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
	const uint32_t windowWidth = 560;
	const uint32_t windowHeight = 460;

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
	::emscripten_set_main_loop_arg(loop, &arg, 0, 1);
#else
	while (!arg.done) {
		loop(&arg);
	}
#endif

	return EXIT_SUCCESS;
}

