#include "loader.hpp"
#include "game.hpp"
#include "options_menu.hpp"

namespace loader
{
	SDL_Window* initialize_engine(const char* title) noexcept
	{
		CONSOLE_LOG("Initialiasing SDL engine");
		// Init SDL Engine
		if (SDL_Init(SDL_INIT_VIDEO) < 0)
		{
			CONSOLE_ERROR("Error initializing video: (%s)", SDL_GetError());
			return NULL;
		}
		CONSOLE_LOG("Initialiasing TTF engine");
		if (TTF_Init() < 0)
		{
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
		if (pWindow == nullptr)
		{
			CONSOLE_ERROR("Error initializing window: (%s)", SDL_GetError());
			return NULL;
		}
		return pWindow;
	}

	uchar quit_engine(const uchar& code) noexcept
	{
		CONSOLE_LOG("Quitting TTF engine");
		TTF_Quit();
		CONSOLE_LOG("Quitting SDL engine");
		SDL_Quit();
		return code;
	}

	uchar quit_engine(SDL_Window* pWindow, const uchar& code) noexcept
	{
		CONSOLE_LOG("Destroying window");
		SDL_DestroyWindow(pWindow);
		return quit_engine(code);
	}

	uchar quit_engine(Assets& assets, Screen& s, const uchar& code) noexcept
	{
		CONSOLE_LOG("Freeing window surface");
		SDL_FreeSurface(s.surface);
		assets.free();
		return quit_engine(s.window, code);
	}

	bool load(TTF_Font** font, const char* name, const uchar& size) noexcept
	{
		CONSOLE_LOG("Loading font %s sized %d", name, size);
		*font = TTF_OpenFont(name, size);
		if (*font == nullptr)
		{
			CONSOLE_ERROR("Ressources loading error: (%s)", TTF_GetError());
			return false;
		}
		return true;
	}

	bool load(SDL_Surface** surface, const char* name) noexcept
	{
		CONSOLE_LOG("Loading texture %s ", name);
		*surface = IMG_Load(name);
		if (*surface == nullptr)
		{
			CONSOLE_ERROR("Ressources loading error: (%s)", IMG_GetError());
			return false;
		}
		return true;
	}

	bool levelLoader(const char* fileName, const uchar& n, const std::function<void(const uchar& i, const uchar& c)>& callback) noexcept
	{
		CONSOLE_LOG("Loading file : %s", fileName);
		FILE* f = fopen(fileName, "r");
		if (f == nullptr)
		{
			CONSOLE_ERROR("Error can't open file : (%s)", strerror(errno));
			return false;
		}

		// Skipping the first levels of the file
		fseek(f, n * (NB_BLOCKS_LENGTH + 2), SEEK_SET);

		char c;
		for (uchar i = 0; i < NB_BLOCKS_LENGTH && (c = getc(f)) != EOF; i++)
			callback(i, c);

		fclose(f);
		return true;
	}
} // namespace loader