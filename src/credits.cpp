#include "libraries.hpp"
#include "loader.hpp"

namespace credits {
	ExitCode loop(const Screen& s, const loader::Assets& assets) noexcept {
		ExitCode exit_code = ExitCode::NONE;

		SDLRect txt_creator = { 203, 131 }, txt_version = { 315, 95 };

		SDL_Event event;

		while (exit_code == ExitCode::NONE) {
			if (SDL_WaitEvent(&event)) {
				switch (event.type) {
				case SDL_EVENT_QUIT:
					exit_code = ExitCode::QUIT;
					break;
				case SDL_EVENT_WINDOW_LAST:
					if (event.window.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED)
						exit_code = ExitCode::QUIT;
					break;
				case SDL_EVENT_KEY_DOWN:
					switch (event.key.key) {
					case SDLK_RETURN:
					case SDLK_ESCAPE:
						exit_code = ExitCode::RETURN;
						break;
					default:
						break;
					}
				}
			}
			SDL_BlitSurface(assets.menu_credits, NULL, s.surface, NULL);
			SDL_BlitSurface(assets.txt_creator, NULL, s.surface, &txt_creator);
			SDL_BlitSurface(assets.txt_version, NULL, s.surface, &txt_version);
			SDL_UpdateWindowSurface(s.window);
		}
		return exit_code;
	}
}; // namespace credits
