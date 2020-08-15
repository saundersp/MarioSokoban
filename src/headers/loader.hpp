#pragma once
#include "global.hpp"

namespace loader
{
	SDL_Window* initialize_engine(const char* title) noexcept;
	bool load(TTF_Font** font, const char* name, const uchar& size) noexcept;
	bool load(SDL_Surface** surface, const char* name) noexcept;
	bool levelLoader(const char* fileName, const uchar& n, const std::function<void(const uchar& i, const uchar& c)>& callback) noexcept;

	struct SpritePlayer
	{
		SDL_Surface* up, * right, * down, * left;
		uchar load_error = 0;

		SpritePlayer(const char* name, const char* ext) noexcept
		{
			CONSOLE_LOG("Creating SpritePlayer named %s", name);
			auto loadDir = [name, ext](SDL_Surface** s, const char* d) {
				char dir[40];
				snprintf(dir, sizeof dir, SPRITE_LOCATION "/%s_%s.%s", name, d, ext);
				bool load_error = !load(s, dir);
				if (*s != nullptr)
					SDL_SetColorKey(*s, SDL_TRUE, SDL_MapRGB((*s)->format, 255, 255, 255));
				return load_error;
			};

			load_error += loadDir(&up, "haut");
			load_error += loadDir(&right, "droite");
			load_error += loadDir(&down, "bas");
			load_error += loadDir(&left, "gauche");
		}

		constexpr void free() noexcept
		{
			for (SDL_Surface* s : { up, right, down, left })
				if (s != nullptr)
					SDL_FreeSurface(s);
		}

		constexpr SDL_Surface* operator[](const Direction& d) const noexcept
		{
			switch (d)
			{
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
	};

	struct Assets
	{
		// Fonts
		TTF_Font* arial_blk_std, * arial_blk_md, * arial_blk_small, * arial_blk_xsmall;

		// Sprites
		SDL_Surface* window_icon, * menu_selector, * congratulations;
		SDL_Surface* cadre, * cadre2, * box, * box_placed, * objective, * wall;

		// Characters sprites
		SpritePlayer* mario, * luigi, * pacman;

		// Menus
		SDL_Surface* menu, * menu_options, * menu_play, * menu_editor, * menu_credits;

		// Text for menu
		SDL_Surface* txt_play, * txt_editor, * txt_options, * txt_credits;
		SDL_Surface* txt_quit, * txt_quit_confirm, * txt_creator, * txt_version;
		SDL_Surface* txt_yes, * txt_no, * txt_repeatkeys, * txt_charac;

		uchar load_error = 0;

		Assets() noexcept
		{
			CONSOLE_LOG("Creating assets");
			CONSOLE_LOG("Loading fonts...");
			// Loading fonts...
			load_error += !load(&arial_blk_std, POLICE_LOCATION "/ariblk.ttf", 30);
			load_error += !load(&arial_blk_md, POLICE_LOCATION "/ariblk.ttf", 20);
			load_error += !load(&arial_blk_small, POLICE_LOCATION "/ariblk.ttf", 15);
			load_error += !load(&arial_blk_xsmall, POLICE_LOCATION "/ariblk.ttf", 14);
			CONSOLE_LOG("Loading Sprites...");
			// Loading sprites
			load_error += !load(&window_icon, DATA_LOCATION "/caisse.ico");
			load_error += !load(&congratulations, SPRITE_LOCATION "/bravo.jpg");
			load_error += !load(&cadre, SPRITE_LOCATION "/cadre.png");
			load_error += !load(&cadre2, SPRITE_LOCATION "/cadre2.png");
			load_error += !load(&box, SPRITE_LOCATION "/caisse.jpg");
			load_error += !load(&wall, SPRITE_LOCATION "/mur.jpg");
			load_error += !load(&box_placed, SPRITE_LOCATION "/caisse_ok.jpg");
			load_error += !load(&menu_selector, SPRITE_LOCATION "/objectif.png");
			if (menu_selector != nullptr)
				SDL_SetColorKey(menu_selector, SDL_TRUE, SDL_MapRGB(menu_selector->format, 255, 255, 255));
			objective = menu_selector;
			CONSOLE_LOG("Loading SpritePlayers...");
			// Loading SpritePlayer
			load_error += (mario = new SpritePlayer("mario", "gif"))->load_error;
			load_error += (luigi = new SpritePlayer("luigi", "png"))->load_error;
			load_error += (pacman = new SpritePlayer("pac", "jpg"))->load_error;
			CONSOLE_LOG("Loading Menus...");
			// Loading menus
			load_error += !load(&menu, MENU_LOCATION "/menu.png");
			load_error += !load(&menu_options, MENU_LOCATION "/options.png");
			load_error += !load(&menu_editor, MENU_LOCATION "/instructions.png");
			load_error += !load(&menu_credits, MENU_LOCATION "/credits.png");
			load_error += !load(&menu_play, MENU_LOCATION "/instructions_jeu.jpg");
			CONSOLE_LOG("Creating Text sprites...");
			// Creating text sprites
			auto txtshd = [](TTF_Font* f, const char* s) { return f == nullptr ? NULL : TTF_RenderUTF8_Shaded(f, s, { 255, 255, 255, 255 }, { 0, 0, 0, 255 }); };
			txt_play = txtshd(arial_blk_std, "Jouer");
			txt_editor = txtshd(arial_blk_std, "Editeur");
			txt_options = txtshd(arial_blk_std, "Options");
			txt_credits = txtshd(arial_blk_std, "Crédits");
			txt_quit = txtshd(arial_blk_std, "Quitter");
			txt_creator = txtshd(arial_blk_md, "Aspilow");
			txt_version = txtshd(arial_blk_md, "V" GAME_VERSION);
			txt_quit_confirm = txtshd(arial_blk_md, "Vous êtes sur(e) ?  Oui/Non");
			txt_charac = txtshd(arial_blk_std, "Perso");
			txt_yes = txtshd(arial_blk_small, "Oui");
			txt_no = txtshd(arial_blk_xsmall, "No");
			txt_repeatkeys = txtshd(arial_blk_std, "Répétition Touches");
			CONSOLE_LOG("Finished creating assets");
		}

		void free() noexcept
		{
			CONSOLE_LOG("Destroying assets");
			CONSOLE_LOG("Destroying Fonts");
			// Fonts
			for (TTF_Font* f : { arial_blk_std, arial_blk_md, arial_blk_small, arial_blk_xsmall })
				if (f != nullptr)
					TTF_CloseFont(f);
			CONSOLE_LOG("Destroying assets");
			CONSOLE_LOG("Destroying Sprites");
			// Sprites
			for (SDL_Surface* s : { window_icon, congratulations, cadre, cadre2, box, box_placed, objective, wall })
				if (s != nullptr)
					SDL_FreeSurface(s);
			CONSOLE_LOG("Destroying assets");
			CONSOLE_LOG("Destroying SpritePlayers");
			// Sprites player
			for (SpritePlayer* p : { mario, luigi, pacman })
				if (p != nullptr)
				{
					p->free();
					delete p;
				}
			CONSOLE_LOG("Destroying assets");
			CONSOLE_LOG("Destroying fonts");
			// Menus
			for (SDL_Surface* s : { menu, menu_selector, menu_options, menu_play, menu_editor })
				if (s != nullptr)
					SDL_FreeSurface(s);
			CONSOLE_LOG("Destroying assets");
			CONSOLE_LOG("Destroying Text sprites");
			// Texts sprites
			for (SDL_Surface* s : { txt_play, txt_editor, txt_options, txt_credits, txt_quit, txt_quit_confirm, txt_creator, txt_version, txt_yes, txt_no, txt_repeatkeys, txt_charac })
				if (s != nullptr)
					SDL_FreeSurface(s);
			CONSOLE_LOG("Done freeing assets !");
		}

		constexpr SDL_Surface* operator[](const SpriteName& n) const noexcept
		{
			switch (n)
			{
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
	};

	uchar quit_engine(const uchar& code) noexcept;
	uchar quit_engine(SDL_Window* pWindow, const uchar& code) noexcept;
	uchar quit_engine(Assets& assets, Screen& s, const uchar& code) noexcept;
} // namespace loader