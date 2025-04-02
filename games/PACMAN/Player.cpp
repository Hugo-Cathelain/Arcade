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
    , mMovementPercentage(1.f)
    , mMovementSpeed(9.47f)
    , mMovementAccumulator(0.f)
    , isDead(false)
{}

///////////////////////////////////////////////////////////////////////////////
void Player::SetMovementPercentage(float percentage)
{
    mMovementPercentage = percentage;
}

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
void Player::DrawDeathAnimation(float timer)
{
    int idx = static_cast<int>((timer * 7.5f)) % 11 * 2;

    if ((idx == 0 && timer > 0.5f) || isDead) {
        isDead = true;
        return;
    }

    API::Draw(
        IGameModule::Asset(
            {17, 16 + idx},
            "XX", CLR_YELLOW, {16, 16}
        ),
        mPosition + Vec2f(0, ARCADE_OFFSET_Y)
    );
}

///////////////////////////////////////////////////////////////////////////////
void Player::Update(float deltaSeconds)
{
    Vec2i currentTile(
        static_cast<int>(std::floor(mPosition.x + 0.5f)),
        static_cast<int>(std::floor(mPosition.y + 0.5f))
    );

    if (mDesiredDirection != mDirection) {
        Vec2i nextTile = currentTile + mDesiredDirection;

        if (nextTile.x < 0) nextTile.x = ARCADE_GAME_WIDTH - 1;
        else if (nextTile.x > (ARCADE_GAME_WIDTH - 1)) nextTile.x = 0;

        if (PACMAN_MAP[nextTile.y][nextTile.x] == TILE_EMPTY) {
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

            if (PACMAN_MAP[checkTile.y][checkTile.x] != TILE_EMPTY) {
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

        if (PACMAN_MAP[checkTile.y][checkTile.x] != TILE_EMPTY) {
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

    mPosition = newPosition;
}

} // namespace Arc::Pacman
