#include "libraries.hpp"
#include "game.hpp"
#include "loader.hpp"
#include "options_menu.hpp"

namespace game {
	constexpr Vector2D Vector2D::operator+(const Direction& d) const noexcept {
		switch (d) {
		case Direction::UP:
			return Vector2D(x, y - 1);
		case Direction::RIGHT:
			return Vector2D(x + 1, y);
		case Direction::LEFT:
			return Vector2D(x - 1, y);
		case Direction::DOWN:
			return Vector2D(x, y + 1);
		default:
			return Vector2D(x, y);
		}
	}

	constexpr Vector2D Vector2D::operator+=(const Direction& d) noexcept {
		return operator=(operator+(d));
	}

	constexpr Vector2D::operator uchar() const noexcept {
		return y * NB_BLOCKS_WIDTH + x;
	}

	constexpr void Player::operator=(const Vector2D& p) noexcept {
		pos = p;
		this->x = pos.x * BLOCK_SIZE;
		this->y = pos.y * BLOCK_SIZE;
	}

	bool load_level(const uchar& n, SpriteName blocks[NB_BLOCKS_LENGTH], Player* p, const bool& isSource) noexcept {
		CONSOLE_LOG("Loading level : %i", n);
		return loader::level_loader(isSource ? DATA_LOCATION "/levels_source.lvl" : DATA_LOCATION "/levels_edited.lvl",
			n, [p, blocks](const uchar& i, const uchar& c) noexcept {
				if (c == '4') {
					*p = Vector2D(i % NB_BLOCKS_WIDTH, i / NB_BLOCKS_HEIGHT);
					blocks[i] = SpriteName::EMPTY;
				}
				else
					blocks[i] = static_cast<SpriteName>(c);
			});
	}

	constexpr void movePlayer(Player& p, const Direction& d, SpriteName blocks[NB_BLOCKS_LENGTH]) noexcept {
		p.direction = d;
		const Vector2D newPos = p.pos + d, newBlockPos = newPos + d;
		switch (blocks[newPos]) {
		case SpriteName::VALID_BLOCK:
		case SpriteName::BLOCK:
			if (blocks[newBlockPos] == SpriteName::EMPTY || blocks[newBlockPos] == SpriteName::OBJECTIVE) {
				blocks[newBlockPos] = blocks[newBlockPos] == SpriteName::OBJECTIVE ? SpriteName::VALID_BLOCK : SpriteName::BLOCK;
				blocks[newPos] = blocks[newPos] == SpriteName::VALID_BLOCK ? SpriteName::OBJECTIVE : SpriteName::EMPTY;
		case SpriteName::OBJECTIVE:
		case SpriteName::EMPTY:
			p = newPos;
			}
			break;
		default:
			break;
		}
	}

	ExitCode screen_loop(const Screen& s, SDL_Surface* surface) noexcept {
		ExitCode exit_code = ExitCode::NONE;
		SDL_Event event;

		while (exit_code == ExitCode::NONE) {
			if (SDL_WaitEvent(&event)) {
				switch (event.type) {
				case SDL_WINDOWEVENT:
					if (event.window.event == SDL_WINDOWEVENT_CLOSE)
				case SDL_QUIT:
					exit_code = ExitCode::QUIT;
					break;
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym) {
					case SDLK_ESCAPE:
						exit_code = ExitCode::RETURN;
						break;
					case SDLK_RETURN:
						exit_code = ExitCode::CONTINUE;
						break;
					}
				}
			}
			SDL_BlitSurface(surface, NULL, s.surface, NULL);
			SDL_UpdateWindowSurface(s.window);
		}
		return exit_code;
	}

	ExitCode loop(const Screen& s, const loader::Assets& assets) noexcept {
		ExitCode exit_code = ExitCode::NONE;
		uchar currentLevel = -1;

		const ExitCode ins_code = screen_loop(s, assets.menu_play);
		if (ins_code != ExitCode::CONTINUE)
			return ins_code;

		const uchar charac = options_menu::load_options().charac;
		loader::SpritePlayer* playerSkin = nullptr;
		switch (charac) {
		default:
			CONSOLE_ERROR("Illegal value of charac : %d", charac);
		case 0:
			playerSkin = assets.mario;
			break;
		case 1:
			playerSkin = assets.pacman;
			break;
		case 2:
			playerSkin = assets.luigi;
			break;
		}
		Player p(0, 0, Direction::RIGHT, playerSkin);
		SpriteName blocks[NB_BLOCKS_LENGTH] = { SpriteName::EMPTY };

		SDL_Surface* lvlDisplay = nullptr;
		SDLRect posLvlDisplay;
		const SDL_Color color_lvlDisplay = { 0, 255, 0, 255 };

		auto makeCurrentLevel = [&](const uchar& n) mutable {
			bool isSource = true;
			uchar real_n = n;
			if (real_n >= NUMBER_LEVELS_DEFAULT) {
				isSource = false;
				real_n -= NUMBER_LEVELS_DEFAULT;
			}
			if (currentLevel != n) {
				currentLevel = n;
				char level[17] = { 0 };
				if (isSource) sprintf(level, "Level : %d", n + 1);
				else sprintf(level, "Custom Level: %d", real_n + 1);
				lvlDisplay = TTF_RenderText_Blended(assets.arial_blk_md, level, color_lvlDisplay);
				posLvlDisplay.x = s.surface->w / 2 - lvlDisplay->w / 2;
				posLvlDisplay.y = s.surface->h - lvlDisplay->h;
			}
			p.direction = charac == 1 ? Direction::RIGHT : Direction::DOWN;
			return load_level(real_n, blocks, &p, isSource);
		};

		if (makeCurrentLevel(0))
			return ExitCode::CRASH;

		SDL_Event event;

		while (exit_code == ExitCode::NONE) {
			if (SDL_WaitEvent(&event)) {
				switch (event.type) {
				case SDL_WINDOWEVENT:
					if (event.window.event == SDL_WINDOWEVENT_CLOSE)
				case SDL_QUIT:
					exit_code = ExitCode::QUIT;
					break;
				case SDL_KEYDOWN:
					if (event.key.repeat == 0)
						switch (event.key.keysym.sym) {
						case SDLK_ESCAPE:
							exit_code = ExitCode::RETURN;
							break;
						case SDLK_r:
							if (makeCurrentLevel(currentLevel))
								exit_code = ExitCode::CRASH;
							break;
						case SDLK_PAGEDOWN:
							if (currentLevel > 0 && makeCurrentLevel(currentLevel - 1))
								exit_code = ExitCode::CRASH;
							break;
						case SDLK_PAGEUP:
							if (makeCurrentLevel(currentLevel + 1))
								exit_code = ExitCode::CRASH;
							break;

						case SDLK_UP:
						case SDLK_RIGHT:
						case SDLK_DOWN:
						case SDLK_LEFT:
							movePlayer(p, (Direction)event.key.keysym.sym, blocks);
							uchar nb_objective = 0;
							for (const SpriteName& val : blocks)
								if (val == SpriteName::OBJECTIVE)
									++nb_objective;
							if (nb_objective == 0) {
								if (makeCurrentLevel(currentLevel + 1))
									return ExitCode::CRASH;
							}
							break;
						}
				}
			}

			SDL_FillRect(s.surface, NULL, SDL_MapRGB(s.surface->format, 0, 0, 0));

			for (uchar y = 0; y < NB_BLOCKS_HEIGHT; ++y)
				for (uchar x = 0; x < NB_BLOCKS_WIDTH; ++x) {
					uchar idx = y * NB_BLOCKS_WIDTH + x;
					if (blocks[idx] != SpriteName::EMPTY) {
						SDLRect pos = { x * BLOCK_SIZE, y * BLOCK_SIZE };
						SDL_BlitSurface(assets[blocks[idx]], NULL, s.surface, &pos);
					}
				}

			SDL_BlitSurface((*p.sprite)[p.direction], NULL, s.surface, &p);
			SDL_BlitSurface(lvlDisplay, NULL, s.surface, &posLvlDisplay);
			SDL_UpdateWindowSurface(s.window);
		}

		return exit_code;
	}
}; // namespace game
