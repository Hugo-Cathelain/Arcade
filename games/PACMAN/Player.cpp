///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "games/PACMAN/Player.hpp"
#include "games/PACMAN/Assets.hpp"
#include "../../Arcade/core/API.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Arc::Pacman
///////////////////////////////////////////////////////////////////////////////
namespace Arc::Pacman
{

///////////////////////////////////////////////////////////////////////////////
Player::Player(void)
    : mDesiredDirection(0)
    , mDirection(0)
    , mPosition(13.5f, 23)
    , mAnimationOffset(0)
    , mDirectionOffset(0)
    , mMovementSpeed(7.5f)
    , mMovementAccumulator(0.f)
{}

///////////////////////////////////////////////////////////////////////////////
void Player::SetPosition(const Vec2i& position)
{
    mPosition = position;
}

///////////////////////////////////////////////////////////////////////////////
Vec2i Player::GetPosition(void) const
{
    return (Vec2i(std::round(mPosition.x), std::round(mPosition.y)));
}

///////////////////////////////////////////////////////////////////////////////
void Player::SetDesiredDirection(const Vec2i& direction)
{
    mDesiredDirection = direction;
}

///////////////////////////////////////////////////////////////////////////////
Vec2i Player::GetDirection(void) const
{
    return (mDirection);
}

///////////////////////////////////////////////////////////////////////////////
void Player::Draw(float timer)
{
    mAnimationOffset = static_cast<int>(timer * 12) % 2 ? 2 : 0;
    mDirectionOffset = 0;

    if (mDirection == Vec2i(0, 0)) {
        mDirectionOffset = 0;
        mAnimationOffset = 4;
    } else if (mDirection.x == 1) {
        mDirectionOffset = 0;
    } else if (mDirection.x == -1) {
        mDirectionOffset = 2;
    } else if (mDirection.y == -1) {
        mDirectionOffset = 4;
    } else if (mDirection.y == 1) {
        mDirectionOffset = 6;
    }

    API::Draw(
        PACMAN_XY(mDirectionOffset, mAnimationOffset),
        mPosition + Vec2f(0, ARCADE_OFFSET_Y)
    );
}

///////////////////////////////////////////////////////////////////////////////
void Player::Update(float deltaSeconds)
{
    float moveThreshold = 1.0f / mMovementSpeed;

    mMovementAccumulator += deltaSeconds;

    if (mMovementAccumulator < moveThreshold) {
        return;
    }

    mMovementAccumulator -= moveThreshold;
    Vec2f nextPosition = mPosition +
        Vec2f(mDesiredDirection.x, mDesiredDirection.y);

    if (nextPosition.x < 0) {
        nextPosition.x = ARCADE_GAME_WIDTH - 1;
    } else if (nextPosition.x >= ARCADE_GAME_WIDTH) {
        nextPosition.x = 0;
    }

    int x = static_cast<int>(nextPosition.x);
    int y = static_cast<int>(nextPosition.y);

    if (PACMAN_MAP[y][x] == TILE_EMPTY) {
        mDirection = mDesiredDirection;
        mPosition = nextPosition;
    } else {
        nextPosition = mPosition + Vec2f(mDirection.x, mDirection.y);
        x = static_cast<int>(nextPosition.x);
        y = static_cast<int>(nextPosition.y);
        if (PACMAN_MAP[y][x] == TILE_EMPTY) {
            mPosition = nextPosition;
        }
    }
}

} // namespace Arc::Pacman
