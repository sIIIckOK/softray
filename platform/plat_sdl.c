#include "../game.c"
#include "SDL2/SDL.h"
#include "stdbool.h"
#include <stdlib.h>

static SDL_Window *window;
static SDL_Renderer *renderer;
static SDL_Texture *texture;

int main(int argc, char **argv){
	char *program_name = shift(&argc ,&argv);
	Screen s = {
	.pixels = screen_pixels,
	.width = SCREEN_WIDTH,
	.height = SCREEN_HEIGHT,
 	};

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
		SDL_Log("SDL_Init failed: %s", SDL_GetError());
				return 1;
		}
	window = SDL_CreateWindow("Hello",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,s.width,s.height,0);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, s.width, s.height);


	game_update(&s);
	bool running = true;
	while (running) {
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				running = false;
				}
			if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_h) {
				if (!screen_to_ppm(&s, "./frames/line.ppm")) {

							exit(69);
					}
				}
			}
		game_update(&s);
		
		SDL_UpdateTexture(
			texture,
			NULL,
			s.pixels,
			s.width * 4
			);


		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);
	}

 	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	
	return 0;

}
