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
    , mNewOffset({1, 0})
    , mPosition({16, 14})
    , mScore(0)
{
    // Initialize snake with head and tail
    mSnakeParts.push_back(mPosition);
    mSnakeParts.push_back(Vec2i{mPosition.x - 2, mPosition.y});
    mSnakeParts.push_back(Vec2i{mPosition.x - 4, mPosition.y});
    mSnakeParts.push_back(Vec2i{mPosition.x - 6, mPosition.y});

    // Initialize random seed for apple placement
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // Place initial apple
    respawnApple();
}

///////////////////////////////////////////////////////////////////////////////
Snake::~Snake()
{}

///////////////////////////////////////////////////////////////////////////////
void Snake::respawnApple(void)
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
void Snake::Text(
    const std::string& text,
    Snake::TextColor color,
    const Vec2i& position
)
{
    static const int LTRS_OFFSET_Y = 8;
    static const char LTRS[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ!@..0123456789/-\"";
    static const int LTRS_ROW_SIZE = 15;
    static const Color LTS_COLORS[] = {
        Color{224, 221, 255}, Color{255, 0, 0}, Color{252, 181, 255},
        Color{0, 255, 255}, Color{248, 187, 85}, Color{250, 185, 176},
        Color{255, 255, 0}
    };

    for (size_t i = 0; i < text.size(); i++) {
        int index = -1;
        for (size_t j = 0; j < sizeof(LTRS); j++) {
            if (text[i] == LTRS[j]) {
                index = j;
                break;
            }
        }

        if (index == -1) {
            continue;
        }

        int row = index / LTRS_ROW_SIZE + static_cast<int>(color) * 4;
        int col = index % LTRS_ROW_SIZE;

        IGameModule::Asset sprite(
            {col, LTRS_OFFSET_Y + row},
            std::string(1, text[i]),
            LTS_COLORS[static_cast<int>(color)]
        );

        API::Draw(sprite, position + Vec2i{static_cast<int>(i), 0});
    }
}

///////////////////////////////////////////////////////////////////////////////
void Snake::handleKeyPressed(EKeyboardKey key)
{
    mNewOffset = mOffset;
    switch (key) {
        case EKeyboardKey::UP:
            mNewOffset = {0, -1};
            break;
        case EKeyboardKey::DOWN:
            mNewOffset = {0, 1};
            break;
        case EKeyboardKey::LEFT:
            mNewOffset = {-1, 0};
            break;
        case EKeyboardKey::RIGHT:
            mNewOffset = {1, 0};
            break;
        default:
            break;
    }
}

///////////////////////////////////////////////////////////////////////////////
void Snake::drawSpritesSnake(void)
{
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
void Snake::moveSnake(void)
{

    // Move snake when accumulated time reaches threshold
    if (mAccumulatedTime >= 0.2f) {
        mAccumulatedTime = 0.0f;

        // Prevent 180-degree turns (can't go directly backwards)
        if (mNewOffset.x != -mOffset.x || mNewOffset.y != -mOffset.y)
            mOffset = mNewOffset;

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
}

///////////////////////////////////////////////////////////////////////////////
void Snake::drawScore(void)
{
    // Draw score
    Text("SCORE", TextColor::TEXT_WHITE, Vec2i{0, 0});
    Text(std::to_string(mScore), TextColor::TEXT_WHITE, Vec2i{6, 0});
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

    if (!mGameOver) {
        // Move snake
        moveSnake();
        // Draw apple
        API::Draw(SPRITES[APPLE], mApplePosition);
        // Draw snake
    } else {
    }
    drawScore();
    drawSpritesSnake();
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
