#include <SDL2/SDL.h>
#include <LuaJIT/lua.hpp>
#include <iostream>
#include <sol.hpp>

#include <neko.hpp>

#define STATUS_OK 0
#define ERROR_FAILED_TO_OPEN_WINDOW 1
#define ERROR_FAILED_TO_CREATE_RENDERER 2

int main() {
	// Init SDL video system
	SDL_Init(SDL_INIT_VIDEO);

	// Open config

	sol::state lua;
	neko_config config;

	try {
		// Run config
		sol::load_result configState = lua.load_file(CONFIG_NAME);
		configState();

		if (lua["config"]) {
			// Config is ok
			// Read window width
			if (lua["config"]["window"]["width"]) {
				config.windowWidth = lua["config"]["window"]["width"];
			}

			// Read window height
			if (lua["config"]["window"]["height"]) {
				config.windowHeight = lua["config"]["window"]["height"];
			}

			// Read canvas width
			if (lua["config"]["window"]["width"]) {
				config.canvasWidth = lua["config"]["canvas"]["width"];
			}

			// Read canvas height
			if (lua["config"]["window"]["height"]) {
				config.canvasHeight = lua["config"]["canvas"]["height"];
			}

			// Read canvas scale
			if (lua["config"]["window"]["scale"]) {
				config.windowWidth = lua["config"]["canvas"]["scale"];
			}
		} else {
			std::cerr << "Invalid config file\n";
		}
	} catch (sol::error error) {
		std::cout << error.what() << "\n";
	}

	// Init neko8
	initNeko(&config);

	// Attempt to open a centred window
	SDL_Window *window = SDL_CreateWindow(
		"neko8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		config.windowWidth, config.windowHeight,
		SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
	);

	if (NOT(window)) {
		// We failed, so there is nothing to do for us, abort
		std::cerr << "Failed to open window, aborting\n";
		SDL_Quit();
		return ERROR_FAILED_TO_OPEN_WINDOW;
	}

	// Attempt to create renderer
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	if (NOT(renderer)) {
		// We failed, so there is nothing to do for us, abort
		std::cerr << "Failed to create a renderer, aborting\n";
		SDL_Quit();
		return ERROR_FAILED_TO_CREATE_RENDERER;
	}

	// Used to get info about events
	SDL_Event event;
	// If true, neko8 should draw next frame
	bool running = true;

	while (running) {
		while (SDL_PollEvent(&event)) {
			// We got some kind-of an event
			switch (event.type) {
				case SDL_QUIT:
					// User closes the window
					running = false;
					// TODO: save here
					break;
				default:
					// Something else, that we don't care about
					break;
			}
		}

		// Clear the window
		SDL_RenderClear(renderer);
		// And sync it
		SDL_RenderPresent(renderer);
	}

	// Free renderer
	SDL_DestroyRenderer(renderer);
	// Free window
	SDL_DestroyWindow(window);
	// And exit
	SDL_Quit();

	return STATUS_OK;
}