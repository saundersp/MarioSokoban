#include "libraries.hpp"
#include "loader.hpp"
#include "game.hpp"
#include "options_menu.hpp"
#include "credits.hpp"
#include "editor.hpp"

namespace main_menu
{
	ExitCode loop(Screen& s, const loader::Assets& assets) noexcept
	{
		uchar menu_selected = 0;
		ExitCode c, exit_code = ExitCode::CONTINUE;
		SDL_Rect menu_selector = { 120, 150, 0, 0 }; // First position of the pointer of the menu
		SDL_Rect txt_play = { 160, 145, 0, 0 }, txt_editor = { 160, 195, 0, 0 }, txt_options = { 160, 245, 0, 0 };
		SDL_Rect txt_credits = { 160, 295, 0, 0 }, txt_quit = { 160, 345, 0, 0 };//, txt_quit_confirm = {204 - 50, 204 - 20, 0, 0};

		SDL_Event event;

		while (exit_code == ExitCode::CONTINUE)
		{
			if (SDL_PollEvent(&event))
			{
				switch (event.type)
				{
				case SDL_WINDOWEVENT:
					if (event.window.event == SDL_WINDOWEVENT_CLOSE)
				case SDL_QUIT:
					exit_code = ExitCode::QUIT;
					break;
				case SDL_KEYDOWN:
					if (event.key.repeat == 0)
						switch (event.key.keysym.sym)
						{
						case SDLK_ESCAPE:
							exit_code = ExitCode::QUIT;
							break;

						case SDLK_RETURN:
							switch (menu_selected)
							{
							case 0:
								c = game::loop(s, assets);
								if (c != ExitCode::RETURN)
									exit_code = c;
								break;
							case 1:
								c = editor::loop(s, assets);
								if (c != ExitCode::RETURN)
									exit_code = c;
								break;
							case 2:
								c = options_menu::loop(s, assets);
								if (c != ExitCode::RETURN)
									exit_code = c;
								break;
							case 3:
								c = credits::loop(s, assets);
								if (c != ExitCode::RETURN)
									exit_code = c;
								break;
							default:
							case 4:
								exit_code = ExitCode::QUIT;
								break;
							}
							break;
						case SDLK_UP:
							if (menu_selected == 0)
							{
								menu_selector.y += 200;
								menu_selected = 4;
							}
							else
							{
								menu_selector.y -= 50;
								menu_selected--;
							}
							break;

						case SDLK_DOWN:
							if (menu_selected == 4)
							{
								menu_selector.y -= 200;
								menu_selected = 0;
							}
							else
							{
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
} // namespace main_menu