#pragma once
#include "loader.hpp"

namespace editor {
	ExitCode loop(const Screen& s, const loader::Assets& assets) noexcept;
};

namespace loader {
	bool save_level(const uchar& n, const SpriteName blocks[NB_BLOCKS_LENGTH]) noexcept;
};