///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "games/SNAKE/Snake.hpp"
#include "Arcade/core/API.hpp"
#include "games/SNAKE/Assets.hpp"
#include <iostream>
#include <deque>
#include <cstdlib>
#include <ctime>

///////////////////////////////////////////////////////////////////////////////
// Namespace Arc
///////////////////////////////////////////////////////////////////////////////
namespace Arc
{

///////////////////////////////////////////////////////////////////////////////
Snake::Snake(void)
    : mGameOver(false)
    , mAccumulatedTime(0.0f)
    , mOffset({1, 0})
    , mPosition({16, 14})
    , mScore(0)
{
    // Initialize snake with head and tail
    mSnakeParts.push_back(mPosition);
    mSnakeParts.push_back({mPosition.x - mOffset.x, mPosition.y - mOffset.y});

    // Initialize random seed for apple placement
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // Place initial apple
    respawnApple();
}

///////////////////////////////////////////////////////////////////////////////
Snake::~Snake()
{}

///////////////////////////////////////////////////////////////////////////////
void Snake::respawnApple()
{
    // Random position for apple
    int x = (std::rand() % 15) * 2;
    int y = (std::rand() % 12) * 2 + 4;

    // Make sure apple doesn't spawn on snake
    for (const auto& part : mSnakeParts) {
        if (part.x == x && part.y == y) {
            respawnApple();
            return;
        }
    }

    mApplePosition = {x, y};
}

///////////////////////////////////////////////////////////////////////////////
void Snake::BeginPlay(void)
{
    API::PushEvent(API::Event::GRAPHICS, API::Event::GridSize({31, 28}));
}

///////////////////////////////////////////////////////////////////////////////
void Snake::EndPlay(void)
{}

///////////////////////////////////////////////////////////////////////////////
void Snake::handleKeyPressed(EKeyboardKey key)
{
    Vec2i newOffset = mOffset;

    switch (key) {
        case EKeyboardKey::UP:
            newOffset = {0, -1};
            break;
        case EKeyboardKey::DOWN:
            newOffset = {0, 1};
            break;
        case EKeyboardKey::LEFT:
            newOffset = {-1, 0};
            break;
        case EKeyboardKey::RIGHT:
            newOffset = {1, 0};
            break;
        default:
            break;
    }

    // Prevent 180-degree turns (can't go directly backwards)
    if (newOffset.x != -mOffset.x || newOffset.y != -mOffset.y)
        mOffset = newOffset;
}

///////////////////////////////////////////////////////////////////////////////
void Snake::Tick(float deltaSeconds)
{
    mAccumulatedTime += deltaSeconds;

    // Process input
    while (auto event = API::PollEvent(API::Event::GAME)) {
        if (auto key = event->GetIf<API::Event::KeyPressed>()) {
            handleKeyPressed(key->code);
        }
    }

    // Clear screen
    for (size_t y = 4; y < 28; y += 2) {
        for (size_t x = 0; x < 31; x += 2) {
            API::Draw(SPRITES[EMPTY], Vec2i(x, y));
        }
    }

    // Move snake when accumulated time reaches threshold
    if (mAccumulatedTime >= 0.2f) {
        mAccumulatedTime = 0.0f;

        // Calculate new head position
        Vec2i newHead = {mSnakeParts.front().x + mOffset.x * 2,
                 mSnakeParts.front().y + mOffset.y * 2};

        // Teleport to opposite side when hitting walls
        if (newHead.x < 0) {
            newHead.x = 30;  // Rightmost position (31-1)
        } else if (newHead.x >= 31) {
            newHead.x = 0;   // Leftmost position
        }

        if (newHead.y < 4) {
            newHead.y = 26;  // Bottom position (28-2)
        } else if (newHead.y >= 28) {
            newHead.y = 4;   // Top position
        }

        // Check for collisions with self (skip the tail since it will move)
        for (size_t i = 0; i < mSnakeParts.size() - 1; i++) {
            if (newHead.x == mSnakeParts[i].x && newHead.y == mSnakeParts[i].y) {
                mGameOver = true;
                return;
            }
        }

        // Add new head
        mSnakeParts.push_front(newHead);

        // Check if snake ate the apple
        if (newHead.x == mApplePosition.x && newHead.y == mApplePosition.y) {
            mScore += 10;
            respawnApple();
        } else {
            // Remove tail if no apple was eaten
            mSnakeParts.pop_back();
        }
    }

    // Draw apple
    API::Draw(SPRITES[APPLE], mApplePosition);

    // Draw snake parts
    for (size_t i = 0; i < mSnakeParts.size(); i++) {
        SpriteType sprite;

        if (i == 0) {
            // Draw head based on current movement direction, not the pending direction
            Vec2i currentDirection;
            if (mSnakeParts.size() > 1) {
                currentDirection = {
                    mSnakeParts[0].x - mSnakeParts[1].x,
                    mSnakeParts[0].y - mSnakeParts[1].y
                };
            } else {
                currentDirection = mOffset;
            }

            if (currentDirection.x > 0) sprite = SNAKE_HEAD_L;
            else if (currentDirection.x < 0) sprite = SNAKE_HEAD_R;
            else if (currentDirection.y > 0) sprite = SNAKE_HEAD_B;
            else sprite = SNAKE_HEAD_T;
        } else if (i == mSnakeParts.size() - 1) {
            // Draw tail
            Vec2i diff = {mSnakeParts[i-1].x - mSnakeParts[i].x,
                          mSnakeParts[i-1].y - mSnakeParts[i].y};

            // Handle teleportation for tail
            if (std::abs(diff.x) > 15) diff.x = diff.x > 0 ? -1 : 1;
            if (std::abs(diff.y) > 12) diff.y = diff.y > 0 ? -1 : 1;

            if (diff.x > 0) sprite = SNAKE_TAIL_R;
            else if (diff.x < 0) sprite = SNAKE_TAIL_L;
            else if (diff.y > 0) sprite = SNAKE_TAIL_T;
            else sprite = SNAKE_TAIL_B;
        } else {
            // Draw body
            Vec2i prevDiff = {mSnakeParts[i-1].x - mSnakeParts[i].x,
                             mSnakeParts[i-1].y - mSnakeParts[i].y};
            Vec2i nextDiff = {mSnakeParts[i].x - mSnakeParts[i+1].x,
                             mSnakeParts[i].y - mSnakeParts[i+1].y};

            // Handle teleportation for prevDiff
            if (std::abs(prevDiff.x) > 15) prevDiff.x = prevDiff.x > 0 ? -1 : 1;
            if (std::abs(prevDiff.y) > 12) prevDiff.y = prevDiff.y > 0 ? -1 : 1;

            // Handle teleportation for nextDiff
            if (std::abs(nextDiff.x) > 15) nextDiff.x = nextDiff.x > 0 ? -1 : 1;
            if (std::abs(nextDiff.y) > 12) nextDiff.y = nextDiff.y > 0 ? -1 : 1;

            // Straight body
            if ((prevDiff.x * nextDiff.x > 0) || (prevDiff.y * nextDiff.y > 0)) {
                if (prevDiff.x != 0) sprite = SNAKE_BODY_RL;
                else sprite = SNAKE_BODY_TB;
            }
            // Corner body
            else {
                if ((prevDiff.x > 0 && nextDiff.y < 0) || (prevDiff.y > 0 && nextDiff.x < 0))
                    sprite = SNAKE_BODY_BR;
                else if ((prevDiff.x < 0 && nextDiff.y < 0) || (prevDiff.y > 0 && nextDiff.x > 0))
                    sprite = SNAKE_BODY_BL;
                else if ((prevDiff.x > 0 && nextDiff.y > 0) || (prevDiff.y < 0 && nextDiff.x < 0))
                    sprite = SNAKE_BODY_TR;
                else
                    sprite = SNAKE_BODY_TL;
            }
        }
        API::Draw(SPRITES[sprite], mSnakeParts[i]);
    }
}

///////////////////////////////////////////////////////////////////////////////
bool Snake::IsGameOver(void) const
{
    return (mGameOver);
}

///////////////////////////////////////////////////////////////////////////////
int Snake::GetScore(void) const
{
    return (mScore);
}

///////////////////////////////////////////////////////////////////////////////
std::string Snake::GetName(void) const
{
    return ("Snake");
}

///////////////////////////////////////////////////////////////////////////////
std::string Snake::GetDescription(void) const
{
    return ("Snake game");
}

///////////////////////////////////////////////////////////////////////////////
std::string Snake::GetSpriteSheet(void) const
{
    return ("assets/SNAKE/sprite.png");
}

} // namespace Arc
