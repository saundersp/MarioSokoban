#pragma once
#include "loader.hpp"

namespace game
{
	struct Vector2D
	{
		uchar x, y;
		constexpr Vector2D(const uchar& x, const uchar& y) noexcept : x{ x }, y{ y } {}

		constexpr Vector2D operator+=(const Direction& d) noexcept
		{
			return operator=(operator+(d));
		}

		constexpr Vector2D operator+(const Direction& d) const noexcept
		{
			switch (d)
			{
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

		constexpr operator uchar() const noexcept
		{
			return y * NB_BLOCKS_WIDTH + x;
		}
	}; // namespace game

	struct Player : SDL_Rect
	{
		Vector2D pos;
		Direction direction;
		const loader::SpritePlayer* sprite;

		constexpr Player(const uchar& x, const uchar& y, const Direction& dir, const loader::SpritePlayer* sp) noexcept
			: SDL_Rect{ x, y, BLOCK_SIZE, BLOCK_SIZE }, pos{ x, y }, direction{ dir }, sprite{ sp } {}

		constexpr void operator=(const Vector2D& p) noexcept
		{
			pos = p;
			this->x = pos.x * BLOCK_SIZE;
			this->y = pos.y * BLOCK_SIZE;
		}
	};
	ExitCode screen_loop(Screen& s, SDL_Surface* surface) noexcept;
	ExitCode loop(Screen& s, const loader::Assets& assets) noexcept;
} // namespace game