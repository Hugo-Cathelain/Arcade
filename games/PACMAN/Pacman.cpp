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
    : mPlayer()
    , mAccumulatedTime(0.0f)
    , mGhosts{
        Ghost(Ghost::Type::BLINKY, {12, 14}),
        Ghost(Ghost::Type::PINKY, {13, 14}),
        Ghost(Ghost::Type::INKY, {14, 14}),
        Ghost(Ghost::Type::CLYDE, {15, 14})
    }
    , mPowerPillActive(false)
    , mPowerPillTimer(0.f)
    , mGums(Gum::GetDefaultGums())
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

    if (mAccumulatedTime >= FRAME * 4.f) {
        mAccumulatedTime = 0.f;
    }

    // Update the player and the ghosts
    mPlayer.Update(deltaSeconds);
    for (int i = 0; i < 4; i++) {
        mGhosts[i].Update(deltaSeconds, mPlayer.GetPosition());
    }

    // Draw the map
    for (size_t y = 0; y < 31; y++) {
        for (size_t x = 0; x < 28; x++) {
            API::Draw(SPRITES[PACMAN_MAP[y][x]], Vec2i(x, y));
        }
    }

    // Draw the pacgums
    for (const auto& [pos, gum] : mGums) {
        if (gum->type == Gum::BIG && mAccumulatedTime < FRAME * 2.f) {
            continue;
        }
        gum->Draw();
    }

    // Draw the player and the ghosts
    mPlayer.Draw();
    for (int i = 0; i < 4; i++) {
        mGhosts[i].Draw();
    }
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

