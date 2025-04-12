///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "games/NIBBLER/Snake.hpp"
#include "games/NIBBLER/Assets.hpp"
#include "Arcade/core/API.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Arc::Nibbler
///////////////////////////////////////////////////////////////////////////////

namespace Arc::Nibbler
{

///////////////////////////////////////////////////////////////////////////////
Snake::Snake(void)
    : mDesiredDirection({1,0})
    , mDirection({1,0})
    , mPosition(15.5f, 29)
    , mAnimationOffset(0)
    , mDirectionOffset(0)
    , mMovementPercentage(1.f)
    , mMovementSpeed(9.47f)
    , mMovementAccumulator(0.f)
    , isDead(false)
    , mSnakeColor(Snake_Color::RED)
    , mLevel(0)
{
    // Initialize snake with head and tail
    mSnakeParts.push_back(mPosition);
    mSnakeParts.push_back(Vec2f{mPosition.x - 1, mPosition.y});
    mSnakeParts.push_back(Vec2f{mPosition.x - 2, mPosition.y});
    mSnakeParts.push_back(Vec2f{mPosition.x - 3, mPosition.y});
    mSnakeParts.push_back(Vec2f{mPosition.x - 4, mPosition.y});
    mSnakeParts.push_back(Vec2f{mPosition.x - 5, mPosition.y});
    mSnakeParts.push_back(Vec2f{mPosition.x - 6, mPosition.y});
    mSnakeParts.push_back(Vec2f{mPosition.x - 7, mPosition.y});
}

///////////////////////////////////////////////////////////////////////////////
void Snake::SetMovementPercentage(float percentage)
{
    mMovementPercentage = percentage;
}

///////////////////////////////////////////////////////////////////////////////
void Snake::SetPosition(const Vec2i& position)
{
    mPosition = position;
}

///////////////////////////////////////////////////////////////////////////////
Vec2i Snake::GetPosition(void) const
{
    return (Vec2i(std::round(mPosition.x), std::round(mPosition.y)));
}

///////////////////////////////////////////////////////////////////////////////
void Snake::SetDesiredDirection(const Vec2i& direction)
{
    mDesiredDirection = direction;
}

///////////////////////////////////////////////////////////////////////////////
Vec2i Snake::GetDirection(void) const
{
    return (mDirection);
}

void Snake::SetLevel(int level)
{
    mLevel = level;
}

///////////////////////////////////////////////////////////////////////////////
void Snake::Draw(float timer)
{
    mAnimationOffset = static_cast<int>(timer * 12) % 2 ? 1 : 0;
    mDirectionOffset = 0;
    int snakeColor = static_cast<int>(mSnakeColor);

    for (size_t i = 0; i < mSnakeParts.size(); i++) {
        SpriteType sprite;

        if (i == 0) {
            if (mDirection.x == 1) {
                sprite = SNAKE_HEAD_RIGHT;
            } else if (mDirection.x == -1) {
                sprite = SNAKE_HEAD_LEFT;
            } else if (mDirection.y == -1) {
                sprite = SNAKE_HEAD_BOTTOM;
            } else {
                sprite = SNAKE_HEAD_TOP;
            }
            auto spriteColor = SPRITES[sprite];
            spriteColor.position.x += snakeColor;
            API::Draw(spriteColor, mSnakeParts[i]);

        } else if (i == mSnakeParts.size() - 1) {
            Vec2f diff = {mSnakeParts[i-1].x - mSnakeParts[i].x,
                          mSnakeParts[i-1].y - mSnakeParts[i].y};

            if (diff.x > 0) sprite = SNAKE_TAIL_LEFT;
            else if (diff.x < 0) sprite = SNAKE_TAIL_RIGHT;
            else if (diff.y > 0) sprite = SNAKE_TAIL_BOTTOM;
            else sprite = SNAKE_TAIL_TOP;

            auto spriteColor = SPRITES[sprite];
            spriteColor.position.x += snakeColor;
            API::Draw(spriteColor, mSnakeParts[i]);

        } else {
            Vec2f prevDiff = {mSnakeParts[i-1].x - mSnakeParts[i].x,
                              mSnakeParts[i-1].y - mSnakeParts[i].y};
            Vec2f nextDiff = {mSnakeParts[i].x - mSnakeParts[i+1].x,
                              mSnakeParts[i].y - mSnakeParts[i+1].y};

            if ((prevDiff.x * nextDiff.x > 0) || (prevDiff.y * nextDiff.y > 0)) {
                if (prevDiff.x != 0) sprite = SNAKE_BODY_H;
                else sprite = SNAKE_BODY_V;
            } else {
                mAnimationOffset = 0;
                if ((prevDiff.x > 0 && nextDiff.y < 0) || (prevDiff.y > 0 && nextDiff.x < 0))
                    sprite = SNAKE_TURN_BOTTOM_LEFT;
                else if ((prevDiff.x < 0 && nextDiff.y < 0) || (prevDiff.y > 0 && nextDiff.x > 0))
                    sprite = SNAKE_TURN_BOTTOM_RIGHT;
                else if ((prevDiff.x > 0 && nextDiff.y > 0) || (prevDiff.y < 0 && nextDiff.x < 0))
                    sprite = SNAKE_TURN_TOP_LEFT;
                else
                    sprite = SNAKE_TURN_TOP_RIGHT;
            }
            auto spriteColor = SPRITES[sprite];
            spriteColor.position.x += snakeColor + mAnimationOffset;
            API::Draw(spriteColor, mSnakeParts[i]);
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
    (void)timer;
    // int idx = static_cast<int>((timer * 7.5f)) % 11 * 2;

    // if ((idx == 0 && timer > 0.5f) || isDead) {
    //     isDead = true;
    //     return;
    // }
}

///////////////////////////////////////////////////////////////////////////////
void Snake::Update(float deltaSeconds)
{
    (void)deltaSeconds;
    Vec2i currentTile(
        static_cast<int>(std::floor(mPosition.x + 0.5f)),
        static_cast<int>(std::floor(mPosition.y + 0.5f))
    );

    if (mDesiredDirection != mDirection) {
        Vec2i nextTile = currentTile + mDesiredDirection;

        if (nextTile.x < 0) nextTile.x = ARCADE_GAME_WIDTH - 1;
        else if (nextTile.x > (ARCADE_GAME_WIDTH - 1)) nextTile.x = 0;

        if (MAPS_PER_LEVEL[mLevel][nextTile.y][nextTile.x] == EMPTY) {
            bool alignedForTurn = false;

            if (mDesiredDirection.x != 0) {
                alignedForTurn = (std::abs(mPosition.y - currentTile.y) < 0.05f);
                if (alignedForTurn) mPosition.y = static_cast<float>(currentTile.y);
            } else {
                alignedForTurn = (std::abs(mPosition.x - currentTile.x) < 0.05f);
                if (alignedForTurn) mPosition.x = static_cast<float>(currentTile.x);
            }

            if (alignedForTurn) {
                mDirection = mDesiredDirection;
                mPosition.x = static_cast<float>(currentTile.x);
                mPosition.y = static_cast<float>(currentTile.y);
            }
        }
    }

    Vec2f movement = Vec2f(mDirection) * deltaSeconds * mMovementSpeed * mMovementPercentage;
    Vec2f newPosition = mPosition + movement;
    Vec2i newpos = {static_cast<int>(std::round(newPosition.x)),
                static_cast<int>(std::round(newPosition.y))};
    std::cout << "Snake Head Position last: (" << newpos.x << ", " << newpos.y << ") | " << std::endl;

    bool xCollision = false, yCollision = false;

    if (movement.x != 0.0f) {
        if (newPosition.x < 0) {
            newPosition.x = ARCADE_GAME_WIDTH - 1;
        } else if (newPosition.x > (ARCADE_GAME_WIDTH - 1)) {
            newPosition.x = 0;
        } else {
            Vec2i checkTile(
                static_cast<int>(
                    std::floor(newPosition.x + 0.5f + 0.3f * mDirection.x)
                ),
                currentTile.y
            );

            if (MAPS_PER_LEVEL[mLevel][checkTile.y][checkTile.x] != EMPTY) {
                xCollision = true;
                newPosition.x = mDirection.x > 0
                    ? std::floor(newPosition.x) + 0.5f
                    : std::ceil(newPosition.x) - 0.5f;
            }
        }
    }

    if (movement.y != 0.0f) {
        Vec2i checkTile(
            currentTile.x,
            static_cast<int>(
                std::floor(newPosition.y + 0.5f + 0.3f * mDirection.y)
            )
        );

        if (MAPS_PER_LEVEL[mLevel][checkTile.y][checkTile.x] != EMPTY) {
            yCollision = true;
            newPosition.y = mDirection.y > 0
                ? std::floor(newPosition.y) + 0.5f
                : std::ceil(newPosition.y) - 0.5f;
        }
    }

    if (xCollision || yCollision) {
        if (xCollision) {
            newPosition.x = static_cast<float>(currentTile.x);
        }
        if (yCollision) {
            newPosition.y = static_cast<float>(currentTile.y);
        }
    }
    // Update snake parts based on the new position
    Vec2i lastpos = {static_cast<int>(std::round(mPosition.x)),
                static_cast<int>(std::round(mPosition.y))};
    std::cout << "Snake Head Position last: (" << lastpos.x << ", " << lastpos.y << ") | " << std::endl;
    mPosition = newPosition;
    // Update snake parts based on the new position
    Vec2i pos = {static_cast<int>(std::round(mPosition.x)),
                static_cast<int>(std::round(mPosition.y))};

    // Debug output for head position and direction
    std::cout << "Snake Head Position new: (" << pos.x << ", " << pos.y << ") | ";
    std::cout << "Direction: (" << mDirection.x << ", " << mDirection.y << ") | ";
    std::cout << "Desired Direction: (" << mDesiredDirection.x << ", " << mDesiredDirection.y << ")" << std::endl;

    // Only update the snake if the head has moved to a new tile position
    if (mPosition.x != mSnakeParts[0].x || mPosition.y != mSnakeParts[0].y) {
        // Move tail parts - each part takes position of part in front of it
        for (size_t i = mSnakeParts.size() - 1; i > 0; --i) {
            mSnakeParts[i] = mSnakeParts[i - 1];
        }

        // Update head position
        mSnakeParts[0] = mPosition;

        // Check for collision with own body (after moved)
        // for (size_t i = 1; i < mSnakeParts.size(); ++i) {
        //     if (mSnakeParts[0].x == mSnakeParts[i].x &&
        //         mSnakeParts[0].y == mSnakeParts[i].y) {
        //         isDead = true;
        //         std::cout << "COLLISION DETECTED: Snake hit itself!" << std::endl;
        //         break;
        //     }
        // }

        // Reset movement accumulator after moving
        mMovementAccumulator = 0.f;
    }
}

} // namespace Arc::Nibbler
