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
#include <systems/IntensificationSystem.hpp>

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

	font = fontManager->loadFontFile("assets/pixel_square.ttf", 24);
	medFont = fontManager->loadFontFile("assets/pixel_square.ttf", 32);
	bigFont = fontManager->loadFontFile("assets/pixel_square.ttf", 40);

	state->currentMap = map.get();
	state->screenWidth = screenWidth;
	state->screenHeight = screenHeight;

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

	auto maybeSpeed = packedAssets->insertFile("assets/speed_upgrade.png");
	if (!maybeSpeed) {
		logger->fatal("Couldn't pack required speed upgrade asset");
		return;
	}

	packedAssets->commitPack();

	this->animalPackRect = *maybeAnimals;
	this->cardPackRect = *maybeCards;
	this->smallCardPackRect = *maybeSmallCards;
	this->trashPackRect = *maybeTrash;
	this->speedUpgradePackRect = *maybeSpeed;

	trashSprite = packedAssets->makeSpritePtr({trashPackRect.x, trashPackRect.y + 1, 100, 100});
	speedUpgradeSprite = packedAssets->makeSpritePtr({speedUpgradePackRect.x + 1, speedUpgradePackRect.y + 1, 64, 64});

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

	pausedText = fontManager->renderText(bigFont, "PRESS SPACE TO UNPAUSE", true, APG::FontRenderMethod::NICE);
	victoryText = fontManager->renderText(bigFont, "YOU SURVIVED!", true, APG::FontRenderMethod::NICE);
	victorySubText = fontManager->renderText(medFont, "SPACE TO KEEP PLAYING", true, APG::FontRenderMethod::NICE);
	failureText = fontManager->renderText(bigFont, "YOU DIED!", true, APG::FontRenderMethod::NICE);
	failureSubText = fontManager->renderText(medFont, "REFRESH TO TRY AGAIN!", true, APG::FontRenderMethod::NICE);
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

	{
		deck = engine->addEntity();
		deck->add<PositionComponent>((screenWidth - cardGapX) * 2 - redCardBack->getWidth(),
									 cardGapY);
		deck->add<RenderableComponent>(redCardBack.get());
		deck->add<DeckComponent>();
		deck->add<ClickableComponent>(SDL_Rect{0, 0, redCardBack->getWidth(), redCardBack->getHeight()});
		deck->add<AnnotationComponent>(
				fontManager->renderText(font, "UPGRADES", false, APG::FontRenderMethod::NICE), true);

		state->upgradeDeck = deck;

		auto deckComponent = deck->getComponent<DeckComponent>();

		auto gunCard1 = engine->addEntity();
		gunCard1->add<PositionComponent>(cardGapX, cardGapY);
		gunCard1->add<RenderableComponent>(blueCardFront.get(), gunUpgradeSprite.get());
		gunCard1->add<ClickableComponent>(SDL_Rect{0, 0, blueCardFront->getWidth(), blueCardFront->getHeight()});
		gunCard1->add<CarryableComponent>(smallBlueCardBack.get(), UpgradeType::TOWER_GUN_UPGRADE,
										  gunUpgradeSprite.get());
		gunCard1->add<CardComponent>();

		state->hand.emplace_back(gunCard1);

		auto gunCard2 = std::make_unique<ashley::Entity>();
		gunCard2->add<PositionComponent>(cardGapX, cardGapY);
		gunCard2->add<RenderableComponent>(blueCardFront.get(), gunUpgradeSprite.get());
		gunCard2->add<ClickableComponent>(SDL_Rect{0, 0, blueCardFront->getWidth(), blueCardFront->getHeight()});
		gunCard2->add<CarryableComponent>(smallBlueCardBack.get(), UpgradeType::TOWER_GUN_UPGRADE,
										  gunUpgradeSprite.get());
		gunCard2->add<CardComponent>();

		auto rocketCard1 = std::make_unique<ashley::Entity>();
		rocketCard1->add<PositionComponent>(cardGapX * 2 + blueCardFront->getWidth(), cardGapY);
		rocketCard1->add<RenderableComponent>(blueCardFront.get(), rocketUpgradeSprite.get());
		rocketCard1->add<ClickableComponent>(SDL_Rect{0, 0, redCardFront->getWidth(), redCardFront->getHeight()});
		rocketCard1->add<CarryableComponent>(smallBlueCardBack.get(), UpgradeType::TOWER_ROCKET_UPGRADE,
											 rocketUpgradeSprite.get());
		rocketCard1->add<CardComponent>();

		// Add upgrade cards to deck
		std::vector<std::unique_ptr<ashley::Entity>> cardEntities;
		cardEntities.emplace_back(std::move(gunCard2));
		cardEntities.emplace_back(std::move(rocketCard1));
		for (int i = 0; i < 4; ++i) {
			auto levelUpgrade = std::make_unique<ashley::Entity>();
			levelUpgrade->add<PositionComponent>(cardGapX * 3 + greenCardFront->getWidth() * 2, cardGapY);
			levelUpgrade->add<RenderableComponent>(greenCardFront.get(), plusUpgradeSprite.get());
			levelUpgrade->add<ClickableComponent>(
					SDL_Rect{0, 0, greenCardFront->getWidth(), greenCardFront->getHeight()});
			levelUpgrade->add<CarryableComponent>(smallGreenCardBack.get(), UpgradeType::LEVEL,
												  plusUpgradeSprite.get());
			levelUpgrade->add<CardComponent>();

			cardEntities.emplace_back(std::move(levelUpgrade));
		}

		for (int i = 0; i < 3; ++i) {
			auto speedUpgrade = std::make_unique<ashley::Entity>();
			speedUpgrade->add<PositionComponent>(cardGapX * 3 + greenCardFront->getWidth() * 2, cardGapY);
			speedUpgrade->add<RenderableComponent>(greenCardFront.get(), speedUpgradeSprite.get());
			speedUpgrade->add<ClickableComponent>(
					SDL_Rect{0, 0, greenCardFront->getWidth(), greenCardFront->getHeight()});
			speedUpgrade->add<CarryableComponent>(smallGreenCardBack.get(), UpgradeType::SPEED,
												  speedUpgradeSprite.get());
			speedUpgrade->add<CardComponent>();

			cardEntities.emplace_back(std::move(speedUpgrade));
		}

		bool shuffledBadly = true;
		while (shuffledBadly) {
			std::shuffle(cardEntities.begin(), cardEntities.end(), state->rand);

			for (int i = 0; i < (int) cardEntities.size(); ++i) {
				if (i >= 2) {
					break;
				}

				auto carryable = cardEntities[i]->getComponent<CarryableComponent>();
				if (carryable->upgradeType == UpgradeType::TOWER_GUN_UPGRADE ||
					carryable->upgradeType == UpgradeType::TOWER_ROCKET_UPGRADE) {
					shuffledBadly = false;
				}
			}
		}

		for (auto &e : cardEntities) {
			deckComponent->cards.emplace_front(std::move(e));
		}
	}

	{
		auto trashEntity = engine->addEntity();
		auto trashRenderable = std::make_unique<RenderableComponent>(trashSprite.get());
		trashEntity->add<PositionComponent>((5), (screenHeight - 20) * 2 - trashSprite->getHeight());
		trashEntity->add<TrashComponent>();
		trashEntity->add<AnnotationComponent>(
				fontManager->renderText(font, "TRASH", false, APG::FontRenderMethod::NICE));
		trashEntity->add<ClickableComponent>(SDL_Rect{0, 0, trashSprite->getWidth(), trashSprite->getHeight()});
		trashEntity->add(std::move(trashRenderable));
	}

	engine->addSystem<IntensificationSystem>(100, state, speedUpgradeSprite.get(), plusUpgradeSprite.get(),
											 greenCardFront.get(), smallGreenCardBack.get());
	state->entitySpawnSystem = engine->addSystem<EntitySpawnSystem>(
			500, 17.5f, state.get(),
			std::vector<APG::SpriteBase *>(
					{giraffe.get(), monkey.get(), rabbit.get(), snake.get(), pig.get()}
			),
			std::vector<MonsterStats>(
					{MonsterStats{18, 2.0f}, MonsterStats{9, 0.85f}, MonsterStats{4, 0.6f}, MonsterStats{14, 1.2f},
					 MonsterStats{25, 3.0f}
					})
	);
	engine->addSystem<RenderSystem>(spriteBatch.get(), 1000, fontManager.get(), font);
	engine->addSystem<CarrySystem>(inputManager.get(), 2500);
	engine->addSystem<WalkingSystem>(3500, state);
	playerInputSystem = engine->addSystem<PlayerInputSystem>(5000, inputManager.get(), camera.get(), state);
	engine->addSystem<TowerUpgradeSystem>(10000, state, gunUpgradeSprite.get(), rocketUpgradeSprite.get(),
										  constructionOverlay.get());
	engine->addSystem<TowerAttackSystem>(15000, gunAttackSprite.get(), rocketAttackSprite.get());
	engine->addSystem<MovementSystem>(17500);
	state->toastSystem = engine->addSystem<ToastSystem>(20000, fontManager.get(), font, bigFont);
	engine->addSystem<DeathSystem>(1000000, state);
}

void Obelisk::render(float deltaTime) {
	glClearColor(0.18f, 0.80f, 0.44f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	camera->update();
	spriteBatch->setProjectionMatrix(camera->combinedMatrix);

	if (elapsed > (60 * 4) && !resumeVictory) {
		if (inputManager->isKeyJustPressed(SDL_SCANCODE_SPACE)) {
			resumeVictory = true;
		} else {
			spriteBatch->begin();
			const auto textX = ((int) screenWidth - (victoryText->getWidth() / 2));
			const auto subTextX = ((int) screenWidth - (victorySubText->getWidth() / 2));
			spriteBatch->draw(victoryText, textX, screenHeight / 2.0f * 2.0f);
			spriteBatch->draw(victorySubText, subTextX, 3 * (screenHeight / 2.0f * 2.0f) / 2);
			spriteBatch->end();
		}
	} else if (state->lives <= 0) {
		spriteBatch->begin();
		const auto textX = ((int) screenWidth - (failureText->getWidth() / 2));
		const auto subTextX = ((int) screenWidth - (failureSubText->getWidth() / 2));
		spriteBatch->draw(failureText, textX, screenHeight / 2.0f * 2.0f);
		spriteBatch->draw(failureSubText, subTextX, 3 * (screenHeight / 2.0f * 2.0f) / 2);
		spriteBatch->end();
	} else if (state->paused) {
		if (inputManager->isKeyJustPressed(SDL_SCANCODE_SPACE)) {
			state->paused = !state->paused;
		}
		spriteBatch->begin();
		const auto textX = ((int) screenWidth - (pausedText->getWidth() / 2));
		spriteBatch->draw(pausedText, textX, screenHeight / 2.0f * 2.0f);
		spriteBatch->end();
	} else {
		elapsed += deltaTime;

		if (state->lives != livesTextNum) {
			const std::string livesTmp = "LIVES: " + std::to_string(state->lives);
			livesText = fontManager->renderText(font, livesTmp, true, APG::FontRenderMethod::NICE);
			livesTextNum = state->lives;
		}

		if (inputManager->isKeyJustPressed(SDL_SCANCODE_SPACE)) {
			state->paused = !state->paused;
		}
		auto positionMapper = ashley::ComponentMapper<PositionComponent>::getMapper();

		for (int i = 0; i < (int) state->hand.size(); ++i) {
			auto card = state->hand[i];
			if (card->hasComponent<CarriedComponent>()) {
				continue;
			}

			auto pos = positionMapper.get(card);
			pos->position.x = 5 * (i + 1) + redCardBack->getWidth() * i;

		}

		map->renderer->renderAllAndUpdate(deltaTime);

		engine->update(deltaTime);

		spriteBatch->begin();
		spriteBatch->draw(livesText, screenWidth - (livesText->getWidth() / 2), 20 + redCardBack->getHeight());
		spriteBatch->end();
	}

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

