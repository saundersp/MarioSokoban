#include "libraries.hpp"
#include "loader.hpp"
#include "options_menu.hpp"
#include "game.hpp"

namespace editor {
	bool load_level(const uchar& n, SpriteName blocks[NB_BLOCKS_LENGTH]) noexcept {
		CONSOLE_LOG("Loading level : %i", n);
		return loader::level_loader(DATA_LOCATION "/levels_edited.lvl", n, [blocks](const uchar& i, const uchar& c) noexcept {
			blocks[i] = static_cast<SpriteName>(c);
			});
	}

	bool save_level(const uchar& n, const SpriteName blocks[NB_BLOCKS_LENGTH]) noexcept {
		CONSOLE_LOG("Saving level : %i", n);
		FILE* file = fopen(DATA_LOCATION "/levels_edited.lvl", "r+");
		if (file == nullptr) {
			CONSOLE_ERROR("Error can't save edited level : (%s)", SDL_GetError());
			return false;
		}

		// Skipping the first levels of the file
		fseek(file, n * (NB_BLOCKS_LENGTH + 2), SEEK_SET);

		for (uchar i = 0; i < NB_BLOCKS_LENGTH; i++)
			fputc((int)blocks[i], file);

		fclose(file);
		return true;
	}

	ExitCode loop(const Screen& s, const loader::Assets& assets) noexcept {
		ExitCode exit_code = ExitCode::NONE;
		uchar current_level = 0;

		const ExitCode ins_code = game::screen_loop(s, assets.menu_editor);
		if (ins_code != ExitCode::CONTINUE)
			return ins_code;

		SpriteName selected_asset = SpriteName::OBJECTIVE;
		bool holdRight = false, holdLeft = false;

		const uchar charac = options_menu::load_options().charac;
		SDL_Surface* p = nullptr;
		switch (charac) {
		default:
			CONSOLE_ERROR("Illegal value of charac : %d", charac);
		case 0:
			p = (*assets.mario)[Direction::DOWN];
			break;
		case 1:
			p = (*assets.pacman)[Direction::RIGHT];
			break;
		case 2:
			p = (*assets.luigi)[Direction::DOWN];
			break;
		}
		SpriteName blocks[NB_BLOCKS_LENGTH] = { SpriteName::EMPTY };

		SDL_Surface* lvlDisplay = nullptr;
		SDLRect posLvlDisplay, posMini = { s.surface->w / 2, s.surface->h / 2 };
		SDL_Color color_lvlDisplay = { 0, 255, 0, 255 };

		auto makeCurrentLevel = [&](const uchar& n) mutable {
			current_level = n;
			char level[17] = { 0 };
			sprintf(level, "Custom Level : %d", n + 1);
			lvlDisplay = TTF_RenderText_Blended(assets.arial_blk_md, level, color_lvlDisplay);
			posLvlDisplay.x = s.surface->w / 2 - lvlDisplay->w / 2;
			posLvlDisplay.y = s.surface->h - lvlDisplay->h;
			return load_level(n, blocks);
		};

		if (makeCurrentLevel(0))
			return ExitCode::CRASH;

		// Seems a bit aggressive smh
		SDL_WarpMouseInWindow(s.window, posMini.x, posMini.y);

		SDL_Event event;

		while (exit_code == ExitCode::NONE) {
			if (SDL_PollEvent(&event)) {
				switch (event.type) {
				case SDL_WINDOWEVENT:
					if (event.window.event == SDL_WINDOWEVENT_CLOSE)
				case SDL_QUIT:
					exit_code = ExitCode::QUIT;
					break;
				case SDL_MOUSEBUTTONDOWN:
					switch (event.button.button) {
					case SDL_BUTTON_LEFT:
						holdLeft = true;
						blocks[posMini.y / BLOCK_SIZE * NB_BLOCKS_WIDTH + posMini.x / BLOCK_SIZE] = selected_asset;
						break;
					case SDL_BUTTON_RIGHT:
						holdRight = true;
						blocks[posMini.y / BLOCK_SIZE * NB_BLOCKS_WIDTH + posMini.x / BLOCK_SIZE] = SpriteName::EMPTY;
						break;
					}
					break;
				case SDL_MOUSEBUTTONUP:
					switch (event.button.button) {
					case SDL_BUTTON_LEFT:
						holdLeft = false;
						break;
					case SDL_BUTTON_RIGHT:
						holdRight = false;
						break;
					}
					break;
				case SDL_MOUSEMOTION:
					posMini.x = event.motion.x;
					posMini.y = event.motion.y;
					if (holdLeft)
						blocks[posMini.y / BLOCK_SIZE * NB_BLOCKS_WIDTH + posMini.x / BLOCK_SIZE] = selected_asset;
					else if (holdRight)
						blocks[posMini.y / BLOCK_SIZE * NB_BLOCKS_WIDTH + posMini.x / BLOCK_SIZE] = SpriteName::EMPTY;
					break;
				case SDL_KEYDOWN:
					if (event.key.repeat == 0)
						switch (event.key.keysym.sym) {
						case SDLK_ESCAPE:
							exit_code = ExitCode::RETURN;
							break;
						case SDLK_r:
							if (makeCurrentLevel(current_level))
								return ExitCode::CRASH;
							else
								break;
						case SDLK_s:
							save_level(current_level, blocks);
							break;
						case SDLK_DOWN:
						case SDLK_PAGEDOWN:
							if (current_level > 0 && makeCurrentLevel(current_level - 1))
								exit_code = ExitCode::CRASH;
							break;
						case SDLK_UP:
						case SDLK_PAGEUP:
							if (makeCurrentLevel(current_level + 1))
								exit_code = ExitCode::CRASH;
							break;

						case SDLK_1:
						case SDLK_2:
						case SDLK_3:
						case SDLK_5:
							selected_asset = (SpriteName)(event.key.keysym.sym - SDLK_0 + (int)SpriteName::EMPTY);
							break;

						case SDLK_KP_1:
						case SDLK_KP_2:
						case SDLK_KP_3:
						case SDLK_KP_5:
							selected_asset = (SpriteName)(event.key.keysym.sym - SDLK_KP_1 + 1 + (int)SpriteName::EMPTY);
							break;

						case SDLK_4:
						case SDLK_KP_4:
							selected_asset = SpriteName::CHARACTER;
							break;
						}
				}
			}

			SDL_FillRect(s.surface, NULL, SDL_MapRGB(s.surface->format, 0, 0, 0));

			for (uchar y = 0; y < NB_BLOCKS_HEIGHT; y++)
				for (uchar x = 0; x < NB_BLOCKS_WIDTH; x++) {
					uchar idx = y * NB_BLOCKS_WIDTH + x;
					if (blocks[idx] != SpriteName::EMPTY) {
						SDLRect pos = { x * BLOCK_SIZE, y * BLOCK_SIZE };
						SDL_BlitSurface(blocks[idx] == SpriteName::CHARACTER ? p : assets[(SpriteName)blocks[idx]], NULL, s.surface, &pos);
					}
				}

			SDL_BlitSurface(selected_asset == SpriteName::CHARACTER ? p : assets[selected_asset], NULL, s.surface, &posMini);
			SDL_BlitSurface(lvlDisplay, NULL, s.surface, &posLvlDisplay);
			SDL_UpdateWindowSurface(s.window);
		}

		return exit_code;
	}
}; // namespace editor