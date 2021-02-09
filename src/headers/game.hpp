#pragma once
#include "loader.hpp"

namespace game {
	struct Vector2D {
		uchar x, y;

		constexpr Vector2D(const uchar& x, const uchar& y) noexcept : x{ x }, y{ y } {}
		constexpr Vector2D operator+(const Direction& d) const noexcept;
		constexpr Vector2D operator+=(const Direction& d) noexcept;
		constexpr operator uchar() const noexcept;
	}; // namespace game

	struct Player : SDLRect {
		Vector2D pos;
		Direction direction;
		const loader::SpritePlayer* sprite;

		constexpr Player(const uchar& x, const uchar& y, const Direction& dir, const loader::SpritePlayer* sp) noexcept
			: SDLRect(x, y), pos{ x, y }, direction{ dir }, sprite{ sp } {}
		constexpr void operator=(const Vector2D& p) noexcept;
	};
	ExitCode screen_loop(const Screen& s, SDL_Surface* surface) noexcept;
	ExitCode loop(const Screen& s, const loader::Assets& assets) noexcept;
}; // namespace game