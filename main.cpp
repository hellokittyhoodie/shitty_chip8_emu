#include "chip8.h"
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

int main() {
	chip8 emulator;
	emulator.init();
	emulator.load("rom.ch8");

	std::thread input([&emulator]() {
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS) != 0) {
			SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
			return 1;
		}

		TTF_Init();

		TTF_Font* font = TTF_OpenFont("verdanab.ttf", 12);

		auto wnd = SDL_CreateWindow("Chip8 emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 64 * 20, 32 * 20, SDL_WINDOW_OPENGL);
		auto render = SDL_CreateRenderer(wnd, -1, SDL_RENDERER_ACCELERATED);
		while (1) {
			SDL_Event event;
			while (SDL_PollEvent(&event)) {
				switch (event.type) {
				case SDL_QUIT:
					SDL_Quit();
					std::exit(0);
					break;
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym) {
					case SDLK_1:
						emulator.keys[0] = 1;
						break;
					case SDLK_2:
						emulator.keys[1] = 1;
						break;
					case SDLK_3:
						emulator.keys[2] = 1;
						break;
					case SDLK_4:
						emulator.keys[3] = 1;
						break;
					case SDLK_q:
						emulator.keys[4] = 1;
						break;
					case SDLK_w:
						emulator.keys[5] = 1;
						break;
					case SDLK_e:
						emulator.keys[6] = 1;
						break;
					case SDLK_r:
						emulator.keys[7] = 1;
						break;
					case SDLK_a:
						emulator.keys[8] = 1;
						break;
					case SDLK_s:
						emulator.keys[9] = 1;
						break;
					case SDLK_d:
						emulator.keys[10] = 1;
						break;
					case SDLK_f:
						emulator.keys[11] = 1;
						break;
					case SDLK_z:
						emulator.keys[12] = 1;
						break;
					case SDLK_x:
						emulator.keys[13] = 1;
						break;
					case SDLK_c:
						emulator.keys[14] = 1; 
						break;
					case SDLK_v:
						emulator.keys[15] = 1;
						break;
					}
					break;
				case SDL_KEYUP:
					switch (event.key.keysym.sym) {
					case SDLK_1:
						emulator.keys[0] = 0;
						break;
					case SDLK_2:
						emulator.keys[1] = 0;
						break;
					case SDLK_3:
						emulator.keys[2] = 0;
						break;
					case SDLK_4:
						emulator.keys[3] = 0;
						break;
					case SDLK_q:
						emulator.keys[4] = 0;
						break;
					case SDLK_w:
						emulator.keys[5] = 0;
						break;
					case SDLK_e:
						emulator.keys[6] = 0;
						break;
					case SDLK_r:
						emulator.keys[7] = 0;
						break;
					case SDLK_a:
						emulator.keys[8] = 0;
						break;
					case SDLK_s:
						emulator.keys[9] = 0;
						break;
					case SDLK_d:
						emulator.keys[10] = 0;
						break;
					case SDLK_f:
						emulator.keys[11] = 0;
						break;
					case SDLK_z:
						emulator.keys[12] = 0;
						break;
					case SDLK_x:
						emulator.keys[13] = 0;
						break;
					case SDLK_c:
						emulator.keys[14] = 0;
						break;
					case SDLK_v:
						emulator.keys[15] = 0;
						break;
					}
					break;
				default:
					break;
				}
			}

			SDL_SetRenderDrawColor(render, 0, 0, 0, SDL_ALPHA_OPAQUE);
			SDL_RenderClear(render);

			for (int i = 0; i < 32; i++) {
				for (int j = 0; j < 64; j++) {
					if (emulator.graphics[j + 64*i]) {
						SDL_SetRenderDrawColor(render, 255, 255, 255, 255);
						SDL_Rect rct{ j * 20, i * 20, 20, 20 };
						SDL_RenderFillRect(render, &rct);
					}
				}
			}
			std::string score_text =
				"PC = " + std::to_string(emulator.PC)
				+ " DT = " + std::to_string(emulator.DT)
				+ " ST = " + std::to_string(emulator.ST)
				+ " I = "  + std::to_string(emulator.I)
				+ " SP = " + std::to_string(emulator.SP)
				+ " V0 = " + std::to_string(emulator.registers[emulator.V0])
				+ " V1 = " + std::to_string(emulator.registers[emulator.V1])
				+ " V2 = " + std::to_string(emulator.registers[emulator.V2])
				+ " V3 = " + std::to_string(emulator.registers[emulator.V3])
				+ " V4 = " + std::to_string(emulator.registers[emulator.V4])
				+ " V5 = " + std::to_string(emulator.registers[emulator.V5])
				+ " V6 = " + std::to_string(emulator.registers[emulator.V6])
				+ " V7 = " + std::to_string(emulator.registers[emulator.V7])
				+ " V8 = " + std::to_string(emulator.registers[emulator.V8])
				+ " V9 = " + std::to_string(emulator.registers[emulator.V9])
				+ " VA = " + std::to_string(emulator.registers[emulator.VA])
				+ " VB = " + std::to_string(emulator.registers[emulator.VB])
				+ " VC = " + std::to_string(emulator.registers[emulator.VC])
				+ " VD = " + std::to_string(emulator.registers[emulator.VD])
				+ " VE = " + std::to_string(emulator.registers[emulator.VE])
				+ " VF = " + std::to_string(emulator.registers[emulator.VF]);
			SDL_Color textColor = { 255, 255, 255, 0 };
			SDL_Surface* textSurface = TTF_RenderText_Solid(font, score_text.c_str(), textColor);
			SDL_Texture* text = SDL_CreateTextureFromSurface(render, textSurface);
			int text_width = textSurface->w;
			int text_height = textSurface->h;
			SDL_FreeSurface(textSurface);
			SDL_Rect renderQuad = { 0, 0, text_width, text_height };
			SDL_RenderCopy(render, text, NULL, &renderQuad);
			SDL_DestroyTexture(text);
			SDL_RenderPresent(render);
		}
					  
	});

	input.detach();

	while (emulator.can_run) {
		emulator.cycle();
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	SDL_Quit();

	return 0;
}