///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "games/PACMAN/Gum.hpp"
#include "Arcade/core/API.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Arc
///////////////////////////////////////////////////////////////////////////////
namespace Arc
{

///////////////////////////////////////////////////////////////////////////////
Gum::Gum(Gum::Type type, const Vec2i& position)
    : type(type)
    , position(position)
{}

///////////////////////////////////////////////////////////////////////////////
void Gum::Render(void) const
{
    int spriteIdx = type == Type::SMALL ? TILE_POINT : TILE_PACGUM;
    API::Draw(SPRITES[spriteIdx], position.x, position.y);
}

///////////////////////////////////////////////////////////////////////////////
std::map<int, std::unique_ptr<Gum>> Gum::GetDefaultGums(void)
{
    static const int WIDTH = 28;
    static const int HEIGHT = 31;

    std::map<int, std::unique_ptr<Gum>> gums;

    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        int x = i % WIDTH;
        int y = i / WIDTH;

        if (y >= 9 && y <= 19 && x != 6 && x != 21) {
            continue;
        }

        if (PACMAN_MAP[y][x] == TILE_EMPTY) {
            gums[i] = std::make_unique<Gum>(Gum::Type::SMALL, Vec2i(x, y));
        }
    }

    gums[3 * WIDTH + 1] = std::make_unique<Gum>(Gum::Type::BIG, Vec2i(1, 3));
    gums[3 * WIDTH + 26] = std::make_unique<Gum>(Gum::Type::BIG, Vec2i(26, 3));
    gums[23 * WIDTH + 1] = std::make_unique<Gum>(Gum::Type::BIG, Vec2i(1, 23));
    gums[23 * WIDTH + 26] = std::make_unique<Gum>(Gum::Type::BIG, Vec2i(26, 23));

    return (gums);
}

} // namespace Arc
