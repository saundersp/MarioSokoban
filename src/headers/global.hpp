#pragma once

#include "libraries.hpp"

#define GAME_VERSION "1.4"
#define BLOCK_SIZE 34 // in pixels
#define NB_BLOCKS_WIDTH 12
#define NB_BLOCKS_HEIGHT 12
#define NB_BLOCKS_LENGTH NB_BLOCKS_WIDTH *NB_BLOCKS_HEIGHT
#define WINDOW_WIDTH BLOCK_SIZE *NB_BLOCKS_WIDTH
#define WINDOW_HEIGHT BLOCK_SIZE *NB_BLOCKS_HEIGHT
#define NUMBER_LEVELS_DEFAULT 20
#define DATA_LOCATION "data"
#define MENU_LOCATION DATA_LOCATION "/menus"
#define POLICE_LOCATION DATA_LOCATION "/polices"
#define SPRITE_LOCATION DATA_LOCATION "/sprites"

typedef unsigned char uchar;

enum class Direction : Sint32 // Character orientation
{
	UP = SDLK_UP,
	RIGHT = SDLK_RIGHT,
	LEFT = SDLK_LEFT,
	DOWN = SDLK_DOWN
};

enum class SpriteName : uchar
{
	EMPTY = 48, // 48 == '0'
	WALL,
	BLOCK,
	OBJECTIVE,
	CHARACTER,
	VALID_BLOCK
};

enum class ExitCode : uchar
{
	NONE = 0,
	CRASH,
	QUIT,
	RETURN,
	CONTINUE
};

struct Screen
{
	SDL_Window* window;
	SDL_Surface* surface;
};

#define CONSOLE_ERROR(s, e) fprintf(stderr, "[%s : %s] " s " at %s:%d\n", __DATE__, __TIME__, e, __FILE__, __LINE__)

#ifdef __DEBUG__
#define CONSOLE_LOG(s, ...) fprintf(stdout, "[%s : %s] " s "\n", __DATE__, __TIME__ __VA_OPT__(, ) __VA_ARGS__)
#else
#define CONSOLE_LOG(s, ...)
#endif