#include "libraries.hpp"
#include "loader.hpp"
#include "game.hpp"
#include "options_menu.hpp"
#include "credits.hpp"
#include "editor.hpp"

enum class Menu : uchar {
	Game = 0, Editor, Options, Credits, Quit
};

Menu operator--(Menu& m, int) {
	if (m == Menu::Game) m = Menu::Quit;
	else m = static_cast<Menu>(static_cast<uchar>(m) - 1);
	return m;
}

Menu operator++(Menu& m, int) {
	if (m == Menu::Quit) m = Menu::Game;
	else m = static_cast<Menu>(static_cast<uchar>(m) + 1);
	return m;
}

namespace main_menu {
	ExitCode loop(const Screen& s, const loader::Assets& assets) noexcept {
		Menu menu_selected = Menu::Game;
		ExitCode c, exit_code = ExitCode::CONTINUE;
		SDLRect menu_selector = { 120, 150 }; // First position of the pointer of the menu
		SDLRect txt_play = { 160, 145 }, txt_editor = { 160, 195 }, txt_options = { 160, 245 };
		SDLRect txt_credits = { 160, 295 }, txt_quit = { 160, 345 };//, txt_quit_confirm = {204 - 50, 204 - 20, 0, 0};

		SDL_Event event;

		while (exit_code == ExitCode::CONTINUE) {
			if (SDL_WaitEvent(&event)) {
				switch (event.type) {
				case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
				case SDL_EVENT_QUIT:
					exit_code = ExitCode::QUIT;
					break;
				case SDL_EVENT_KEY_DOWN:
					if (event.key.repeat == 0)
						switch (event.key.key) {
						case SDLK_ESCAPE:
							exit_code = ExitCode::QUIT;
							break;

						case SDLK_RETURN:
							switch (menu_selected) {
							case Menu::Game:
								c = game::loop(s, assets);
								if (c != ExitCode::RETURN)
									exit_code = c;
								break;
							case Menu::Editor:
								c = editor::loop(s, assets);
								if (c != ExitCode::RETURN)
									exit_code = c;
								break;
							case Menu::Options:
								c = options_menu::loop(s, assets);
								if (c != ExitCode::RETURN)
									exit_code = c;
								break;
							case Menu::Credits:
								c = credits::loop(s, assets);
								if (c != ExitCode::RETURN)
									exit_code = c;
								break;
							default:
							case Menu::Quit:
								exit_code = ExitCode::QUIT;
								break;
							}
							break;
						case SDLK_UP:
							if (menu_selected == Menu::Game) {
								menu_selector.y += 200;
								menu_selected = Menu::Quit;
							}
							else {
								menu_selector.y -= 50;
								menu_selected--;
							}
							break;

						case SDLK_DOWN:
							if (menu_selected == Menu::Quit) {
								menu_selector.y -= 200;
								menu_selected = Menu::Game;
							}
							else {
								menu_selector.y += 50;
								menu_selected++;
							}
							break;
						}
				}
			}
			SDL_BlitSurface(assets.menu, NULL, s.surface, NULL);
			SDL_BlitSurface(assets.menu_selector, NULL, s.surface, &menu_selector);
			SDL_BlitSurface(assets.txt_play, NULL, s.surface, &txt_play);
			SDL_BlitSurface(assets.txt_editor, NULL, s.surface, &txt_editor);
			SDL_BlitSurface(assets.txt_options, NULL, s.surface, &txt_options);
			SDL_BlitSurface(assets.txt_credits, NULL, s.surface, &txt_credits);
			SDL_BlitSurface(assets.txt_quit, NULL, s.surface, &txt_quit);
			SDL_UpdateWindowSurface(s.window);
		}

		return exit_code;
	}
}; // namespace main_menu
