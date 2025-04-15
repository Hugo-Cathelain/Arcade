///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "games/NIBBLER/Snake.hpp"
#include "games/NIBBLER/Assets.hpp"
#include "games/NIBBLER/Maps/Map.hpp"
#include "Arcade/core/API.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>

///////////////////////////////////////////////////////////////////////////////
// Namespace Arc::Nibbler
///////////////////////////////////////////////////////////////////////////////

namespace Arc::Nibbler
{

///////////////////////////////////////////////////////////////////////////////
Snake::Snake(void)
    : mDesiredDirection({1,0})
    , mDirection({1,0})
    , mAnimationOffset(0)
    , mDirectionOffset(0)
    , mMovementPercentage(1.f)
    , mMovementSpeed(15.47f)
    , mMovementAccumulator(0.f)
    , isDead(false)
    , mSnakeColor(Color::RED)
    , mLevel(0)
    , mLives(3)
    , mLastEatenTime(0)
    , mSize(0)
{
    Vec2f basePosition(15.f, 29);

    mSnakeParts.push_back(Snake::Part{basePosition, mDirection, mDirection});
    mSnakeParts.push_back(Snake::Part{basePosition - Vec2f(1.f, 0.f),
            mDirection, mDirection});
    mSnakeParts.push_back(Snake::Part{basePosition - Vec2f(2.f, 0.f),
            mDirection, mDirection});
    mSnakeParts.push_back(Snake::Part{basePosition - Vec2f(3.f, 0.f),
            mDirection, mDirection});
    mSnakeParts.push_back(Snake::Part{basePosition - Vec2f(4.f, 0.f),
            mDirection, mDirection});
    mSnakeParts.push_back(Snake::Part{basePosition - Vec2f(5.f, 0.f),
            mDirection, mDirection});
    mSnakeParts.push_back(Snake::Part{basePosition - Vec2f(6.f, 0.f),
            mDirection, mDirection});
    mSnakeParts.push_back(Snake::Part{basePosition - Vec2f(7.f, 0.f),
            mDirection, mDirection});
}

///////////////////////////////////////////////////////////////////////////////
void Snake::SetMovementPercentage(float percentage)
{
    mMovementPercentage = percentage;
}

///////////////////////////////////////////////////////////////////////////////
void Snake::SetPosition(const Vec2i& position)
{
    mSnakeParts[0].position = position;
}

///////////////////////////////////////////////////////////////////////////////
Vec2i Snake::GetPosition(void) const
{
    return (Vec2i(
        std::floor(mSnakeParts[0].position.x + 0.5f),
        std::floor(mSnakeParts[0].position.y + 0.5f)
    ));
}

///////////////////////////////////////////////////////////////////////////////
Vec2i Snake::GetPosition(size_t i) const
{
    return (Vec2i(
        std::floor(mSnakeParts[i].position.x + 0.5f),
        std::floor(mSnakeParts[i].position.y + 0.5f)
    ));
}

///////////////////////////////////////////////////////////////////////////////
size_t Snake::GetSize(void) const
{
    return (mSnakeParts.size());
}

///////////////////////////////////////////////////////////////////////////////
void Snake::SetDesiredDirection(const Vec2i& direction)
{
    if (direction == -mSnakeParts[0].direction) {
        return;
    }

    if (direction == mSnakeParts[0].direction) {
        return;
    }

    if (direction == mSnakeParts[0].desired) {
        return;
    }

    mSnakeParts[0].desired = direction;
}

///////////////////////////////////////////////////////////////////////////////
Vec2i Snake::GetDirection(void) const
{
    return (mDirection);
}

///////////////////////////////////////////////////////////////////////////////
int Snake::GetLives(void) const
{
    return (mLives);
}

///////////////////////////////////////////////////////////////////////////////
void Snake::SetLevel(int level)
{
    mLevel = level;
}

///////////////////////////////////////////////////////////////////////////////
void Snake::Draw(float timer)
{
    mAnimationOffset = static_cast<int>(timer * 12) % 2 ? 1 : 0;
    mDirectionOffset = 0;
    int snakeColor = static_cast<int>(mSnakeColor) * 8;

    for (size_t i = 0; i < mSnakeParts.size(); i++) {
        SpriteType sprite;
        if (i == 0) {
            if (mSnakeParts[0].direction.x == 1) {
                sprite = SNAKE_HEAD_RIGHT;
            } else if (mSnakeParts[0].direction.x == -1) {
                sprite = SNAKE_HEAD_LEFT;
            } else if (mSnakeParts[0].direction.y == -1) {
                sprite = SNAKE_HEAD_TOP;
            } else {
                sprite = SNAKE_HEAD_BOTTOM;
            }
            auto spriteColor = SPRITES[sprite];
            spriteColor.position.x += snakeColor;
            API::Draw(spriteColor, mSnakeParts[i].position);

        } else if (i == mSnakeParts.size() - 1) {
            Vec2f diff = {mSnakeParts[i-1].position.x - mSnakeParts[i].position.x,
                          mSnakeParts[i-1].position.y - mSnakeParts[i].position.y};

            if (diff.x > 0.8f) sprite = SNAKE_TAIL_LEFT;
            else if (diff.x < -0.5f) sprite = SNAKE_TAIL_RIGHT;
            else if (diff.y > 0.5f) sprite = SNAKE_TAIL_TOP;
            else sprite = SNAKE_TAIL_BOTTOM;

            auto spriteColor = SPRITES[sprite];
            spriteColor.position.x += snakeColor;
            API::Draw(spriteColor, mSnakeParts[i].position);

        } else {
            Vec2i prevDiff = {
                static_cast<int>(std::round(mSnakeParts[i-1].position.x -
                    mSnakeParts[i].position.x)),
                static_cast<int>(std::round(mSnakeParts[i-1].position.y -
                    mSnakeParts[i].position.y))
            };
            Vec2i nextDiff = {
                static_cast<int>(std::round(mSnakeParts[i].position.x -
                    mSnakeParts[i+1].position.x)),
                static_cast<int>(std::round(mSnakeParts[i].position.y -
                    mSnakeParts[i+1].position.y))
            };

            if ((prevDiff.x * nextDiff.x > 0) || (prevDiff.y * nextDiff.y > 0)) {
                if (prevDiff.x != 0) sprite = SNAKE_BODY_H;
                else sprite = SNAKE_BODY_V;
            } else {
                mAnimationOffset = 0;
                if ((prevDiff.x > 0 && nextDiff.y < 0) || (prevDiff.y > 0 && nextDiff.x < 0))
                    sprite = SNAKE_TURN_TOP_RIGHT;
                else if ((prevDiff.x < 0 && nextDiff.y < 0) || (prevDiff.y > 0 && nextDiff.x > 0))
                    sprite = SNAKE_TURN_TOP_LEFT;
                else if ((prevDiff.x > 0 && nextDiff.y > 0) || (prevDiff.y < 0 && nextDiff.x < 0))
                    sprite = SNAKE_TURN_BOTTOM_LEFT;
                else
                    sprite = SNAKE_TURN_BOTTOM_RIGHT;
            }
            auto spriteColor = SPRITES[sprite];
            spriteColor.position.x += snakeColor + mAnimationOffset;
            API::Draw(spriteColor, mSnakeParts[i].position);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void Snake::DrawSpawnAnimation(float timer)
{
    (void)timer;
}

///////////////////////////////////////////////////////////////////////////////
void Snake::DrawDeathAnimation(float timer)
{
    if (mSize == 0) {
        mSize = static_cast<int>(mSnakeParts.size());
    }

    float progress = timer / 5.f;
    int toRemove = mSize * progress;
    int nextCount = mSize - toRemove;
    int n = mSnakeParts.size() - nextCount;

    for (int i = 0; i < n; i++) {
        mSnakeParts.pop_back();
    }
}

///////////////////////////////////////////////////////////////////////////////
void Snake::SetColor(Snake::Color color)
{
    mSnakeColor = color;
}

///////////////////////////////////////////////////////////////////////////////
void Snake::Grow(void)
{
    mLastEatenTime = 0;
    Snake::Part lastPart = mSnakeParts[mSnakeParts.size() - 1];
    mSnakeParts.push_back(lastPart);
}

///////////////////////////////////////////////////////////////////////////////
float Snake::GetLastEaten(void) const
{
    return (mLastEatenTime);
}

///////////////////////////////////////////////////////////////////////////////
void Snake::Update(float deltaSeconds)
{
    mLastEatenTime += deltaSeconds;
    float speedInc = 1 + ((mLevel + 1) * 0.2f) + ((mSnakeParts.size() - 6) * 0.01f); // Adjust multiplier as needed

    mMovementAccumulator += mMovementSpeed * mMovementPercentage * speedInc * deltaSeconds;
    const std::vector<std::vector<Arc::SpriteType>>& level = MAPS[mLevel % MAPS.size()];
    static bool hasChanged = true;

    // Only move when accumulated enough movement
    if (mMovementAccumulator >= 1.0f) {
        mMovementAccumulator = 0.0f; // Reset accumulator

        // Process head movement and direction changes
        Snake::Part& head = mSnakeParts[0];
        Vec2i headGrid = Vec2i(
            std::floor(head.position.x + 0.5f),
            std::floor(head.position.y + 0.5f)
        );

        // Check if head can change direction
        if (head.desired != Vec2i(0) && head.desired != -head.direction) {
            Vec2i nextPos = headGrid + head.desired;

            if (level[nextPos.x][nextPos.y - ARCADE_OFFSET_Y] == EMPTY) {
                head.direction = head.desired;
                API::PlaySound(SFX_TURN);
                head.desired = Vec2i(0);
                hasChanged = true;
            }
        }

        // Create trail of previous positions for body parts to follow
        std::vector<Vec2f> prevPositions;
        prevPositions.reserve(mSnakeParts.size());

        // Store current positions before moving
        for (const auto& part : mSnakeParts) {
            prevPositions.push_back(part.position);
        }

        // Move head
        Vec2i nextHeadPos = headGrid + head.direction;

        // Check if head hits wall (for auto-turning)
        if (level[nextHeadPos.x][nextHeadPos.y - ARCADE_OFFSET_Y] != EMPTY) {
            std::vector<Vec2i> directions = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
            std::vector<Vec2i> valids;

            for (const auto& dir : directions) {
                Vec2i test = headGrid + dir;

                if (dir == -head.direction) {
                    continue; // Can't reverse direction
                }

                if (level[test.x][test.y - ARCADE_OFFSET_Y] == EMPTY) {
                    valids.push_back(dir);
                }
            }

            if (valids.size() == 1) {
                head.direction = valids[0];
                nextHeadPos = headGrid + head.direction;
                hasChanged = true;
            } else {
                head.desired = Vec2i(0);
                if (hasChanged) {
                    API::PlaySound(SFX_WRONG);
                    hasChanged = false;
                }
                return;
            }
        }

        // Move head to new position, ensure it's perfectly grid-aligned
        head.position = Vec2f(nextHeadPos);

        // Move body parts to follow the part ahead of them
        for (size_t i = 1; i < mSnakeParts.size(); i++) {
            Snake::Part& part = mSnakeParts[i];

            // Each part takes the position of the part ahead of it
            part.position = prevPositions[i-1];

            // Calculate new direction based on movement
            Vec2i curr = Vec2i(
                std::floor(part.position.x + 0.5f),
                std::floor(part.position.y + 0.5f)
            );

            Vec2i prev = Vec2i(
                std::floor(prevPositions[i].x + 0.5f),
                std::floor(prevPositions[i].y + 0.5f)
            );

            Vec2i diff = curr - prev;

            if (diff != Vec2i(0)) {
                part.direction = diff;
            }
        }
    }
}

void Snake::Reset(void)
{
    mSnakeParts.clear();
    Vec2f basePosition(15.f, 29);
    mLastEatenTime = 0;

    mDirection = {1, 0};
    mSnakeParts.push_back(Snake::Part{basePosition, mDirection, mDirection});
    mSnakeParts.push_back(Snake::Part{basePosition - Vec2f(1.f, 0.f),
            mDirection, mDirection});
    mSnakeParts.push_back(Snake::Part{basePosition - Vec2f(2.f, 0.f),
            mDirection, mDirection});
    mSnakeParts.push_back(Snake::Part{basePosition - Vec2f(3.f, 0.f),
            mDirection, mDirection});
    mSnakeParts.push_back(Snake::Part{basePosition - Vec2f(4.f, 0.f),
            mDirection, mDirection});
    mSnakeParts.push_back(Snake::Part{basePosition - Vec2f(5.f, 0.f),
            mDirection, mDirection});
    mSnakeParts.push_back(Snake::Part{basePosition - Vec2f(6.f, 0.f),
            mDirection, mDirection});
    mSnakeParts.push_back(Snake::Part{basePosition - Vec2f(7.f, 0.f),
            mDirection, mDirection});
    mDesiredDirection = {1, 0};
    mAnimationOffset = 0;
    mDirectionOffset = 0;
    mMovementPercentage = 1.f;
    mMovementSpeed = 10.47f;
    mMovementAccumulator = 0.f;
    mSize = 0;
}

} // namespace Arc::Nibbler
