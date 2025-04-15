///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "games/NIBBLER/Maps.hpp"
#include "games/NIBBLER/Maps/Map.hpp"
#include "games/NIBBLER/Assets.hpp"
#include "Arcade/core/API.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Arc::Nibbler
///////////////////////////////////////////////////////////////////////////////

namespace Arc::Nibbler
{

///////////////////////////////////////////////////////////////////////////////
Maps::Maps(void)
    : mLevel(0)
    , mBorderColor(Border_Color::WHITE)
{}

///////////////////////////////////////////////////////////////////////////////
void Maps::SetLevel(int level)
{
    mLevel = level;
    switch ((level - 1) % 4) {
        case 0:
            mBorderColor = Border_Color::WHITE;
            break;
        case 1:
            mBorderColor = Border_Color::GREEN;
            break;
        case 2:
            mBorderColor = Border_Color::PINK;
            break;
        case 3:
            mBorderColor = Border_Color::BLUE;
            break;
    }
    // mBorderColor = static_cast<Border_Color>(level % 4);
}

///////////////////////////////////////////////////////////////////////////////
int Maps::GetLevel(void) const
{
    return (mLevel);
}

///////////////////////////////////////////////////////////////////////////////
void Maps::DrawMap(int level)
{
    int borderColor = static_cast<int>(mBorderColor);

    // Draw the map based on the level
    for (int y = 0; y < 27; y++) {
        for (int x = 0; x < 27; x++) {
            auto sprite = SPRITES[MAPS[level % MAPS.size()][x][y]];
            sprite.position.x += borderColor;
            API::Draw(sprite, Vec2i{x, y + ARCADE_OFFSET_Y});
        }
    }
}

}; // namespace Arc::Nibbler
