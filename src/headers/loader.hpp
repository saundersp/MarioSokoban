#pragma once
#include "global.hpp"

namespace loader {
	struct SpritePlayer {
		SDL_Surface* up, * right, * down, * left;
		uchar load_error = 0;

		SpritePlayer(const char* name, const char* ext) noexcept;
		constexpr void free() const noexcept;
		SDL_Surface* operator[](const Direction& d) const noexcept;
	};

	struct Assets {
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

		Assets() noexcept;
		void free() const noexcept;
		SDL_Surface* operator[](const SpriteName& n) const noexcept;
	};

	SDL_Window* initialize_engine(const char* title) noexcept;
	uchar quit_engine(const uchar& code) noexcept;
	uchar quit_engine(SDL_Window* pWindow, const uchar& code) noexcept;
	uchar quit_engine(Assets& assets, Screen& s, const uchar& code) noexcept;
	bool load(TTF_Font** font, const char* name, const uchar& size) noexcept;
	bool load(SDL_Surface** surface, const char* name) noexcept;
	bool level_loader(const char* fileName, const uchar& n, const std::function<void(const uchar& i, const uchar& c)>& callback) noexcept;
}; // namespace loader