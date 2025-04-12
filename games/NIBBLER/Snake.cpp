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
    : mDesiredDirection(0)
    , mDirection({1,0})
    , mPosition(13.5f, 23)
    , mAnimationOffset(0)
    , mDirectionOffset(0)
    , mMovementPercentage(1.f)
    , mMovementSpeed(9.47f)
    , mMovementAccumulator(0.f)
    , isDead(false)
    , mSnakeColor(Snake_Color::RED)
    , mLevel(0)
{
    Vec2i pos = {static_cast<int>(mPosition.x), static_cast<int>(mPosition.y)};
    // Initialize snake with head and tail
    mSnakeParts.push_back(pos);
    mSnakeParts.push_back(Vec2i{pos.x - 1, pos.y});
    mSnakeParts.push_back(Vec2i{pos.x - 2, pos.y});
    mSnakeParts.push_back(Vec2i{pos.x - 3, pos.y});
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
                sprite = SNAKE_HEAD_LEFT;
            } else if (mDirection.x == -1) {
                sprite = SNAKE_HEAD_RIGHT;
            } else if (mDirection.y == -1) {
                sprite = SNAKE_HEAD_BOTTOM;
            } else {
                sprite = SNAKE_HEAD_TOP;
            }
            auto spriteColor = SPRITES[sprite];
            spriteColor.position.x += snakeColor;
            API::Draw(spriteColor, mSnakeParts[i]);

        } else if (i == mSnakeParts.size() - 1) {
            Vec2i diff = {mSnakeParts[i-1].x - mSnakeParts[i].x,
                          mSnakeParts[i-1].y - mSnakeParts[i].y};

            if (diff.x > 0) sprite = SNAKE_TAIL_RIGHT;
            else if (diff.x < 0) sprite = SNAKE_TAIL_LEFT;
            else if (diff.y > 0) sprite = SNAKE_TAIL_BOTTOM;
            else sprite = SNAKE_TAIL_TOP;

            auto spriteColor = SPRITES[sprite];
            spriteColor.position.x += snakeColor;
            API::Draw(spriteColor, mSnakeParts[i]);

        } else {
            Vec2i prevDiff = {mSnakeParts[i-1].x - mSnakeParts[i].x,
                              mSnakeParts[i-1].y - mSnakeParts[i].y};
            Vec2i nextDiff = {mSnakeParts[i].x - mSnakeParts[i+1].x,
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

}

} // namespace Arc::Nibbler
