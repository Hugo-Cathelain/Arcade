///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "games/PACMAN/Ghost.hpp"
#include "games/PACMAN/Assets.hpp"
#include "../../Arcade/core/API.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Arc::Pacman
///////////////////////////////////////////////////////////////////////////////
namespace Arc::Pacman
{

///////////////////////////////////////////////////////////////////////////////
Ghost::Ghost(Type type)
    : mType(type)
    , mPosition(static_cast<int>(type) + 12, 14)
    , mDirection(0, 0)
{}

///////////////////////////////////////////////////////////////////////////////
void Ghost::Update(float deltaSeconds)
{
    (void)deltaSeconds;
}

///////////////////////////////////////////////////////////////////////////////
void Ghost::Draw(float timer)
{
    static const Color GHOST_COLORS[] = {
        CLR_RED,
        CLR_PINK,
        CLR_CYAN,
        CLR_ORANGE
    };

    int index = static_cast<int>(mType);
    int flickering = static_cast<int>(timer * 8) % 2 ? 2 : 0;
    int directionOffset = 0;

    if (mDirection.x == 1) {
        directionOffset = 0;
    } else if (mDirection.x == -1) {
        directionOffset = 4;
    } else if (mDirection.y == 1) {
        directionOffset = 8;
    } else if (mDirection.y == -1) {
        directionOffset = 12;
    }

    API::Draw(
        IGameModule::Asset(
            {12 + directionOffset + flickering, index * 2},
            "/\\", GHOST_COLORS[index], {16, 16},
            index * 4
        ),
        mPosition + Vec2i(0, ARCADE_OFFSET_Y)
    );
}

} // namespace Arc::Pacman
