#pragma once
#include "loader.hpp"

namespace editor
{
	ExitCode loop(Screen& s, const loader::Assets& assets) noexcept;
}

namespace loader
{
	bool saveLevel(const uchar& n, const SpriteName blocks[NB_BLOCKS_LENGTH]) noexcept;
}