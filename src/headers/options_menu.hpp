#pragma once
#include "loader.hpp"

namespace options_menu
{
	struct Options
	{
		uchar charac;
		bool keyRepeat;
	};
	ExitCode loop(Screen& s, const loader::Assets& assets) noexcept;
	Options loadOptions();
	bool saveOptions(const Options& opt) noexcept;
} // namespace options_menu