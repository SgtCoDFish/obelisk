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
	const auto logger = el::Loggers::getLogger("APG");

	shaderProgram = APG::ShaderProgram::fromFiles(vertexShaderFilename, fragmentShaderFilename);

	camera = std::make_unique<APG::Camera>(screenWidth, screenHeight);
	constexpr const auto zoom = 2.0f;
	camera->setZoom(zoom);
	camera->setToOrtho(false, screenWidth, screenHeight);
	spriteBatch = std::make_unique<APG::SpriteBatch>(shaderProgram.get());

	tmxRenderer = std::make_unique<APG::PackedTmxRenderer>("assets/map.tmx", spriteBatch.get(), 2048, 1024);
	glm::vec2 tmxRendererPos{
			(this->screenWidth - (tmxRenderer->getPixelWidth() / zoom)),
			(this->screenHeight - (tmxRenderer->getPixelHeight() / zoom)),
	};
	tmxRenderer->setPosition(tmxRendererPos);

	auto glError = glGetError();
	if (glError != GL_NO_ERROR) {
		while (glError != GL_NO_ERROR) {
			logger->fatal("Error in OpenGL while loading: ", APG::prettyGLError(glError));
			glError = glGetError();
		}

		return false;
	}

	return true;
}

void Obelisk::render(float deltaTime) {
	glClearColor(0.18f, 0.80f, 0.44f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	//camera->position.x = cameraX + screenWidth / 2.0f;
	//camera->position.y = cameraY + screenHeight / 2.0f;

	camera->update();
	spriteBatch->setProjectionMatrix(camera->combinedMatrix);

	tmxRenderer->renderAllAndUpdate(deltaTime);

	SDL_GL_SwapWindow(window.get());
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
	const uint32_t windowWidth = 640;
	const uint32_t windowHeight = 480;

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

