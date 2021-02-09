#pragma once
#include "loader.hpp"

namespace options_menu {
	struct Options {
		uchar charac;
		bool keyRepeat;
	};
	ExitCode loop(const Screen& s, const loader::Assets& assets) noexcept;
	Options load_options() noexcept;
	bool save_options(const Options& opt) noexcept;
}; // namespace options_menu