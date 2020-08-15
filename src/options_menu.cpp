#include "libraries.hpp"
#include "options_menu.hpp"
#include "loader.hpp"

namespace options_menu
{
	Options loadOptions()
	{
		CONSOLE_LOG("Loading options");
		FILE* file = fopen(DATA_LOCATION "/options.dat", "r");
		if (file == nullptr)
		{
			CONSOLE_ERROR("Can't load options file : (%s)", strerror(errno));
			return { 0, false };
		}
		uchar charac = fgetc(file) - 48; // 48 == '0'
		bool keyrepeat = (fgetc(file) - 48) == 1;
		CONSOLE_LOG("Loaded: character = %d - keyrepeat = %d", charac, keyrepeat);
		fclose(file);
		return { charac, keyrepeat };
	}

	bool saveOptions(const Options& opt) noexcept
	{
		CONSOLE_LOG("Saving options ...");
		FILE* file = fopen(DATA_LOCATION "/options.dat", "w");
		if (file == nullptr)
		{
			CONSOLE_ERROR("Can't save options file : (%s)", strerror(errno));
			return false;
		}
		fputc(opt.charac + 48, file); // 48 == '0'
		fputc(opt.keyRepeat + 48, file);
		fclose(file);
		CONSOLE_LOG("Saved successfully");
		return true;
	}

	ExitCode loop(Screen& s, const loader::Assets& assets) noexcept
	{
		ExitCode exit_code = ExitCode::NONE;

		const SDL_Rect txt_yes = { 219, 310, 0, 0 }, txt_no = { 148, 310, 0, 0 }, txt_repeatkeys = { 52, 243, 0, 0 }, txt_charac = { 21, 180, 0, 0 };
		const SDL_Rect optMario = { 143, 186, 0, 0 }, optPacman = { 217, 186, 0, 0 }, optLuigi = { 301, 186, 0, 0 };
		const SDL_Rect boxMario = { 139, 183, 0, 0 }, boxPacman = { 214, 183, 0, 0 }, boxLuigi = { 291, 183, 0, 0 };
		const SDL_Rect boxKeyOff = { 139, 302, 0, 0 }, boxKeyOn = { 214, 302, 0, 0 };
		SDL_Rect point1 = { boxMario.x, 203, 0, 0 }, point2 = { 0, 322, 0, 0 }, boxSelected = boxMario;

		SDL_Event event;

		Options opt = loadOptions();
		switch (opt.charac)
		{
		default:
			CONSOLE_ERROR("Illegal value saved of opt.charac : %d", opt.charac);
			opt.charac = 0;
		case 0:
			// Use initial values
			break;
		case 1:
			point1.x = boxPacman.x;
			boxSelected = boxPacman;
			break;
		case 2:
			point1.x = boxLuigi.x;
			boxSelected = boxLuigi;
			break;
		}
		point2.x = !opt.keyRepeat ? boxKeyOff.x : boxKeyOn.x;

		uchar selectedLine = 0;

		auto refreshSelectedCharac = [&]() {
			switch (opt.charac)
			{
			case 0:
				boxSelected = boxMario;
				break;
			case 1:
				boxSelected = boxPacman;
				break;
			case 2:
				boxSelected = boxLuigi;
				break;
			default:
				CONSOLE_ERROR("Illegal value of opt.charac : %d", opt.charac);
				exit_code = ExitCode::CRASH;
				break;
			}
			point1.x = boxSelected.x + 2;
		};
		auto refreshSelectedKeyMode = [&]() {
			boxSelected = opt.keyRepeat ? boxKeyOn : boxKeyOff;
			point2.x = boxSelected.x + 2;
		};
		auto draw = [s](SDL_Surface* su, SDL_Rect r) { SDL_BlitSurface(su, NULL, s.surface, &r); };
		while (exit_code == ExitCode::NONE)
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
							exit_code = ExitCode::RETURN;
							break;
						case SDLK_UP:
						case SDLK_DOWN:
							switch (selectedLine)
							{
							case 0:
								selectedLine = 1;
								refreshSelectedKeyMode();
								break;
							case 1:
								selectedLine = 0;
								refreshSelectedCharac();
								break;
							default:
								CONSOLE_ERROR("Illegal value of selectedLine : %d", selectedLine);
								exit_code = ExitCode::CRASH;
								break;
							}
							break;
						case SDLK_LEFT:
							switch (selectedLine)
							{
							case 0:
								opt.charac = opt.charac == 0 ? 2 : opt.charac - 1;
								refreshSelectedCharac();
								break;
							case 1:
								opt.keyRepeat = !opt.keyRepeat;
								refreshSelectedKeyMode();
								break;
							default:
								CONSOLE_ERROR("Illegal value of selectedLine : %d", selectedLine);
								exit_code = ExitCode::CRASH;
								break;
							}
							break;
						case SDLK_RIGHT:
							switch (selectedLine)
							{
							case 0:
								opt.charac = opt.charac == 2 ? 0 : opt.charac + 1;
								refreshSelectedCharac();
								break;
							case 1:
								opt.keyRepeat = !opt.keyRepeat;
								refreshSelectedKeyMode();
								break;
							default:
								CONSOLE_ERROR("Illegal value of selectedLine : %d", selectedLine);
								exit_code = ExitCode::CRASH;
								break;
							}
							break;
						}
				}
			}
			SDL_FillRect(s.surface, NULL, SDL_MapRGB(s.surface->format, 0, 0, 0));
			SDL_BlitSurface(assets.menu_options, NULL, s.surface, NULL);
			for (const SDL_Rect& p : { point1, point2 })
				draw(assets.objective, p);
			for (const SDL_Rect& p : { boxMario, boxPacman, boxLuigi, boxKeyOff, boxKeyOn })
				draw(assets.cadre, p);
			draw(assets.cadre2, boxSelected);
			draw(assets.txt_charac, txt_charac);
			draw(assets.mario->down, optMario);
			draw(assets.pacman->right, optPacman);
			draw(assets.luigi->down, optLuigi);
			draw(assets.txt_repeatkeys, txt_repeatkeys);
			draw(assets.txt_yes, txt_yes);
			draw(assets.txt_no, txt_no);
			SDL_UpdateWindowSurface(s.window);
		}
		saveOptions(opt);
		return exit_code;
	}
} // namespace options_menu