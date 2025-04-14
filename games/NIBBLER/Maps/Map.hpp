///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "../../../Arcade/utils/Vec2.hpp"
#include "games/NIBBLER/Assets.hpp"
#include "games/NIBBLER/Maps/Map01.hpp"
#include "games/NIBBLER/Maps/Map02.hpp"
#include <vector>

namespace Arc::Nibbler
{

static const std::vector<std::vector<std::vector<SpriteType>>> MAPS = {
    MAP_01,
    MAP_02,
    // Add more maps here
};

static const std::vector<std::vector<Vec2i>> MAPS_FRUIT = {
    MAP_01_FRUIT,
    MAP_02_FRUIT,
    // Add more fruit positions here
};

} // namespace Arc::Nibbler
