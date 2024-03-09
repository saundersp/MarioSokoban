#include "loader.hpp"
#include "game.hpp"
#include "options_menu.hpp"

namespace loader {
	SpritePlayer::SpritePlayer(const char* name, const char* ext) noexcept {
		CONSOLE_LOG("Creating SpritePlayer named %s", name);
		auto loadDir = [name, ext](SDL_Surface** s, const char* d) noexcept {
			char dir[40];
			snprintf(dir, sizeof dir, SPRITE_LOCATION "/%s_%s.%s", name, d, ext);
			bool load_error = load(s, dir);
			if (*s != nullptr)
				SDL_SetColorKey(*s, SDL_TRUE, SDL_MapRGB((*s)->format, 255, 255, 255));
			return load_error;
		};

		load_error += loadDir(&up, "haut");
		load_error += loadDir(&right, "droite");
		load_error += loadDir(&down, "bas");
		load_error += loadDir(&left, "gauche");
	}

	constexpr void SpritePlayer::free() const noexcept {
		for (SDL_Surface* s : { up, right, down, left })
			if (s != nullptr)
				SDL_FreeSurface(s);
	}

	SDL_Surface* SpritePlayer::operator[](const Direction& d) const noexcept {
		switch (d) {
		case Direction::UP:
			return up;
		case Direction::RIGHT:
			return right;
		case Direction::LEFT:
			return left;
		case Direction::DOWN:
			return down;
		default:
			return nullptr;
		}
	}

	Assets::Assets() noexcept {
		CONSOLE_LOG("Creating assets");
		CONSOLE_LOG("Loading fonts...");
		load_error += load(&arial_blk_std, POLICE_LOCATION "/ariblk.ttf", 30);
		load_error += load(&arial_blk_md, POLICE_LOCATION "/ariblk.ttf", 20);
		load_error += load(&arial_blk_small, POLICE_LOCATION "/ariblk.ttf", 15);
		load_error += load(&arial_blk_xsmall, POLICE_LOCATION "/ariblk.ttf", 14);
		CONSOLE_LOG("Loading Sprites...");
		load_error += load(&window_icon, DATA_LOCATION "/caisse.ico");
		load_error += load(&cadre, SPRITE_LOCATION "/cadre.png");
		load_error += load(&cadre2, SPRITE_LOCATION "/cadre2.png");
		load_error += load(&box, SPRITE_LOCATION "/caisse.jpg");
		load_error += load(&wall, SPRITE_LOCATION "/mur.jpg");
		load_error += load(&box_placed, SPRITE_LOCATION "/caisse_ok.jpg");
		load_error += load(&menu_selector, SPRITE_LOCATION "/objectif.png");
		objective = SDL_CreateRGBSurface(menu_selector->flags, menu_selector->w, menu_selector->h, 32, 0, 0, 0, 0); // Allocating memory before the deep copy
		SDL_BlitSurface(menu_selector, NULL, objective, NULL); // Deep copy the surface
		CONSOLE_LOG("Loading SpritePlayers...");
		load_error += (mario = new SpritePlayer("mario", "gif"))->load_error;
		load_error += (luigi = new SpritePlayer("luigi", "png"))->load_error;
		load_error += (pacman = new SpritePlayer("pac", "png"))->load_error;
		CONSOLE_LOG("Loading Menus...");
		load_error += load(&menu, MENU_LOCATION "/menu.png");
		load_error += load(&menu_options, MENU_LOCATION "/options.png");
		load_error += load(&menu_editor, MENU_LOCATION "/instructions.png");
		load_error += load(&menu_credits, MENU_LOCATION "/credits.png");
		load_error += load(&menu_play, MENU_LOCATION "/instructions_jeu.png");
		CONSOLE_LOG("Creating Text sprites...");
		auto txtshd = [](TTF_Font* f, const char* s) noexcept { return f == nullptr ? NULL : TTF_RenderUTF8_Shaded(f, s, { 255, 255, 255, 255 }, { 0, 0, 0, 255 }); };
		txt_play = txtshd(arial_blk_std, "Jouer");
		txt_editor = txtshd(arial_blk_std, "Editeur");
		txt_options = txtshd(arial_blk_std, "Options");
		txt_credits = txtshd(arial_blk_std, "Crédits");
		txt_quit = txtshd(arial_blk_std, "Quitter");
		txt_creator = txtshd(arial_blk_md, "Pierre Saunders");
		txt_version = txtshd(arial_blk_md, "V" GAME_VERSION);
		txt_quit_confirm = txtshd(arial_blk_md, "Vous êtes sur(e) ? Oui/Non");
		txt_charac = txtshd(arial_blk_std, "Perso");
		txt_yes = txtshd(arial_blk_small, "Oui");
		txt_no = txtshd(arial_blk_xsmall, "No");
		txt_repeatkeys = txtshd(arial_blk_std, "Répétition Touches");
		CONSOLE_LOG("Finished creating assets");
	}

	void Assets::free() const noexcept {
		CONSOLE_LOG("Destroying assets");
		CONSOLE_LOG("Destroying Fonts");
		for (TTF_Font* f : { arial_blk_std, arial_blk_md, arial_blk_small, arial_blk_xsmall })
			if (f != nullptr)
				TTF_CloseFont(f);
		CONSOLE_LOG("Destroying assets");
		CONSOLE_LOG("Destroying Sprites");
		for (SDL_Surface* s : { window_icon, cadre, cadre2, box, box_placed, objective, wall })
			if (s != nullptr)
				SDL_FreeSurface(s);
		CONSOLE_LOG("Destroying assets");
		CONSOLE_LOG("Destroying SpritePlayers");
		for (SpritePlayer* p : { mario, luigi, pacman })
			if (p != nullptr) {
				p->free();
				delete p;
			}
		CONSOLE_LOG("Destroying assets");
		CONSOLE_LOG("Destroying fonts");
		for (SDL_Surface* s : { menu, menu_selector, menu_options, menu_credits, menu_play, menu_editor })
			if (s != nullptr)
				SDL_FreeSurface(s);
		CONSOLE_LOG("Destroying assets");
		CONSOLE_LOG("Destroying Text sprites");
		for (SDL_Surface* s : { txt_play, txt_editor, txt_options, txt_credits, txt_quit, txt_quit_confirm, txt_creator, txt_version, txt_yes, txt_no, txt_repeatkeys, txt_charac })
			if (s != nullptr)
				SDL_FreeSurface(s);
		CONSOLE_LOG("Done freeing assets !");
	}

	SDL_Surface* Assets::operator[](const SpriteName& n) const noexcept {
		switch (n) {
		case SpriteName::WALL:
			return wall;
		case SpriteName::BLOCK:
			return box;
		case SpriteName::OBJECTIVE:
			return objective;
		case SpriteName::VALID_BLOCK:
			return box_placed;
		case SpriteName::EMPTY:
		case SpriteName::CHARACTER:
		default:
			return NULL;
		}
	}

	SDL_Window* initialize_engine(const char* title) noexcept {
		CONSOLE_LOG("Initialiasing SDL engine");
		if (SDL_Init(SDL_INIT_VIDEO) < 0) {
			CONSOLE_ERROR("Error initializing video: (%s)", SDL_GetError());
			return NULL;
		}
		CONSOLE_LOG("Initialiasing TTF engine");
		if (TTF_Init() < 0) {
			CONSOLE_ERROR("Error initializing TTF: (%s)", TTF_GetError());
			SDL_Quit();
			return NULL;
		}
		CONSOLE_LOG("Creating window named \"%s\"", title);
		SDL_Window* pWindow = SDL_CreateWindow(title,
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			WINDOW_WIDTH, WINDOW_HEIGHT,
			SDL_WINDOW_SHOWN);
		if (pWindow == nullptr) {
			CONSOLE_ERROR("Error initializing window: (%s)", SDL_GetError());
			return NULL;
		}
		return pWindow;
	}

	uchar quit_engine(const uchar& code) noexcept {
		CONSOLE_LOG("Quitting TTF engine");
		TTF_Quit();
		CONSOLE_LOG("Quitting SDL engine");
		SDL_Quit();
		return code;
	}

	uchar quit_engine(SDL_Window* pWindow, const uchar& code) noexcept {
		CONSOLE_LOG("Destroying window");
		SDL_DestroyWindow(pWindow);
		return quit_engine(code);
	}

	uchar quit_engine(Assets& assets, Screen& s, const uchar& code) noexcept {
		CONSOLE_LOG("Freeing window surface");
		SDL_FreeSurface(s.surface);
		assets.free();
		return quit_engine(s.window, code);
	}

	bool load(TTF_Font** font, const char* name, const uchar& size) noexcept {
		CONSOLE_LOG("Loading font %s sized %d", name, size);
		*font = TTF_OpenFont(name, size);
		if (*font == nullptr) {
			CONSOLE_ERROR("Ressources loading error: (%s)", TTF_GetError());
			return true;
		}
		return false;
	}

	bool load(SDL_Surface** surface, const char* name) noexcept {
		CONSOLE_LOG("Loading texture %s ", name);
		*surface = IMG_Load(name);
		if (*surface == nullptr) {
			CONSOLE_ERROR("Ressources loading error: (%s)", IMG_GetError());
			return true;
		}
		return false;
	}

	bool level_loader(const char* fileName, const uchar& n, const std::function<void(const uchar& i, const uchar& c)>& callback) noexcept {
		CONSOLE_LOG("Loading file : %s", fileName);
		FILE* f = fopen(fileName, "r");
		if (f == nullptr) {
			CONSOLE_ERROR("Error can't open file : (%s)", strerror(errno));
			return true;
		}

		// Skipping the first levels of the file
		fseek(f, n * (NB_BLOCKS_LENGTH + 1), SEEK_SET);

		char c;
		for (uchar i = 0; i < NB_BLOCKS_LENGTH && (c = getc(f)) != EOF; i++)
			callback(i, c);

		fclose(f);
		return false;
	}
}; // namespace loader
