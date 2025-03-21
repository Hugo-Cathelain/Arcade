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
    : mPacmanX(1)
    , mPacmanY(1)
    , mPacmanOffsetX(0)
    , mPacmanOffsetY(0)
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
    mAccumulatedTime += deltaSeconds;

    while (auto event = API::PollEvent(API::Event::GAME)) {
        if (auto key = event->GetIf<API::Event::KeyPressed>()) {
            switch (key->code) {
                case EKeyboardKey::UP:
                    mPacmanY--;
                    mPacmanOffsetX = 4;
                    break;
                case EKeyboardKey::DOWN:
                    mPacmanY++;
                    mPacmanOffsetX = 6;
                    break;
                case EKeyboardKey::LEFT:
                    mPacmanX--;
                    mPacmanOffsetX = 2;
                    break;
                case EKeyboardKey::RIGHT:
                    mPacmanX++;
                    mPacmanOffsetX = 0;
                    break;
                default:
                    break;
            }
        }
    }

    for (size_t y = 0; y < PACMAN_MAP.size(); y++) {
        for (size_t x = 0; x < PACMAN_MAP[y].size(); x++) {
            API::Draw(PACMAN_MAP[y][x], x, y);
        }
    }

    if (mAccumulatedTime >= 0.5f) {
        mAccumulatedTime = 0.0f;
    }

    mPacmanOffsetY = mAccumulatedTime < 0.25f ? 0 : 2;

    API::Draw(
        SPRITE_PACMAN_ALL(mPacmanOffsetX, mPacmanOffsetY),
        mPacmanX, mPacmanY
    );
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
    return ("PACMAN.png");
}

} // namespace Arc

