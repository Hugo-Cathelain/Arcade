///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "games/PACMAN/Pacman.hpp"
#include "Arcade/core/API.hpp"
#include "games/PACMAN/Assets.hpp"
#include <iostream>

///////////////////////////////////////////////////////////////////////////////
// Namespace Arc
///////////////////////////////////////////////////////////////////////////////
namespace Arc
{

///////////////////////////////////////////////////////////////////////////////
Pacman::Pacman(void)
    : mPosition({1, 1})
    , mOffset({0, 0})
    , mAccumulatedTime(0.0f)
{}

///////////////////////////////////////////////////////////////////////////////
Pacman::~Pacman()
{}

///////////////////////////////////////////////////////////////////////////////
void Pacman::BeginPlay(void)
{
    API::PushEvent(API::Event::GRAPHICS, API::Event::GridSize({28, 31}));
}

///////////////////////////////////////////////////////////////////////////////
void Pacman::EndPlay(void)
{}

///////////////////////////////////////////////////////////////////////////////
void Pacman::Tick(float deltaSeconds)
{
    Vec2 oldPosition = mPosition;
    mAccumulatedTime += deltaSeconds;

    while (auto event = API::PollEvent(API::Event::GAME)) {
        if (auto key = event->GetIf<API::Event::KeyPressed>()) {
            switch (key->code) {
                case EKeyboardKey::UP:
                    mPosition.y--;
                    mOffset.x = 4;
                    break;
                case EKeyboardKey::DOWN:
                    mPosition.y++;
                    mOffset.x = 6;
                    break;
                case EKeyboardKey::LEFT:
                    mPosition.x--;
                    mOffset.x = 2;
                    break;
                case EKeyboardKey::RIGHT:
                    mPosition.x++;
                    mOffset.x = 0;
                    break;
                default:
                    break;
            }
        }
    }

    if (PACMAN_MAP[mPosition.y][mPosition.x] != TILE_EMPTY) {
        if (mPosition.x < 0) {
            mPosition.x = 27;
        } else if (mPosition.x > 27) {
            mPosition.x = 0;
        } else {
            mPosition = oldPosition;
        }
    }

    for (size_t y = 0; y < 31; y++) {
        for (size_t x = 0; x < 28; x++) {
            API::Draw(SPRITES[PACMAN_MAP[y][x]], x, y);
        }
    }

    if (mAccumulatedTime >= 0.5f) {
        mAccumulatedTime = 0.0f;
    }

    mOffset.y = mAccumulatedTime < 0.25f ? 0 : 2;

    API::Draw(PACMAN_XY(mOffset.x, mOffset.y), mPosition.x, mPosition.y);
}

///////////////////////////////////////////////////////////////////////////////
bool Pacman::IsGameOver(void) const
{
    return (false);
}

///////////////////////////////////////////////////////////////////////////////
int Pacman::GetScore(void) const
{
    return (0);
}

///////////////////////////////////////////////////////////////////////////////
std::string Pacman::GetName(void) const
{
    return ("Pacman");
}

///////////////////////////////////////////////////////////////////////////////
std::string Pacman::GetDescription(void) const
{
    return ("Pacman game");
}

///////////////////////////////////////////////////////////////////////////////
std::string Pacman::GetSpriteSheet(void) const
{
    return ("assets/PACMAN/sprites.png");
}

} // namespace Arc

