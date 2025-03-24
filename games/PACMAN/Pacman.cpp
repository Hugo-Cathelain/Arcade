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
    : mPosition({14, 17})
    , mOffset({0, 0})
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
void Pacman::handleKeyPressed(EKeyboardKey key)
{
    Vec2i oldPosition = mPosition;

    switch (key) {
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

    if (PACMAN_MAP[mPosition.y][mPosition.x] != TILE_EMPTY) {
        if (mPosition.x < 0) {
            mPosition.x = 27;
        } else if (mPosition.x > 27) {
            mPosition.x = 0;
        } else {
            mPosition = oldPosition;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void Pacman::Tick(float deltaSeconds)
{
    mAccumulatedTime += deltaSeconds;

    while (auto event = API::PollEvent(API::Event::GAME)) {
        if (auto key = event->GetIf<API::Event::KeyPressed>()) {
            handleKeyPressed(key->code);
        }
    }

    for (int i = 0; i < 4; i++) {
        mGhosts[i].Update(deltaSeconds, mPosition);
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

    std::vector<int> toDelete;

    for (const auto& [pos, gum] : mGums) {
        gum->Render();

        int x = pos % 28;
        int y = pos / 28;

        if (mPosition.x == x && mPosition.y == y) {
            toDelete.push_back(pos);
        }
    }

    for (int pos : toDelete) {
        mGums.erase(pos);
    }

    API::Draw(PACMAN_XY(mOffset.x, mOffset.y), mPosition.x, mPosition.y);

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

