///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "games/PACMAN/Player.hpp"
#include "Arcade/core/API.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Arc
///////////////////////////////////////////////////////////////////////////////
namespace Arc
{

///////////////////////////////////////////////////////////////////////////////
const Vec2i Player::STARTING_POSITION = Vec2i(14.f, 17.f);

///////////////////////////////////////////////////////////////////////////////
Player::Player(void)
    : mPosition(STARTING_POSITION)
    , mDirection(0)
    , mNextDirection(0)
    , mAccumulatedTime(0.f)
    , mMovementSpeed(6.f)
    , mMovementAccumulator(0.f)
{}

///////////////////////////////////////////////////////////////////////////////
void Player::HandleEvents(void)
{
    while (auto event = API::PollEvent(API::Event::GAME)) {
        if (auto key = event->GetIf<API::Event::KeyPressed>()) {
            if (key->code == EKeyboardKey::DOWN) {
                mNextDirection = Vec2i(0, 1);
            } else if (key->code == EKeyboardKey::UP) {
                mNextDirection = Vec2i(0, -1);
            } else if (key->code == EKeyboardKey::LEFT) {
                mNextDirection = Vec2i(-1, 0);
            } else if (key->code == EKeyboardKey::RIGHT) {
                mNextDirection = Vec2i(1, 0);
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void Player::Update(float deltaSeconds)
{
    // Update animation timer
    mAccumulatedTime += deltaSeconds;
    if (mAccumulatedTime >= FRAME) {
        mAccumulatedTime = 0.0f;
    }

    // Handle input events
    HandleEvents();

    // Update movement with frame-independent timing
    mMovementAccumulator += deltaSeconds;

    // Move when enough time has passed based on speed
    float moveThreshold = 1.0f / mMovementSpeed;
    if (mMovementAccumulator >= moveThreshold) {
        // Reset timer (preserving any excess time)
        mMovementAccumulator -= moveThreshold;

        // Move in the direction of the next direction if possible
        Vec2i nextPosition = mPosition + mNextDirection;

        if (nextPosition.x < 0) {
            nextPosition.x = MAP_WIDTH - 1;
        } else if (nextPosition.x >= MAP_WIDTH) {
            nextPosition.x = 0;
        }

        // Check if the next position is empty
        if (PACMAN_MAP[nextPosition.y][nextPosition.x] == TILE_EMPTY) {
            mDirection = mNextDirection;
            mPosition = nextPosition;
        } else {
            // Check if the current position is empty
            nextPosition = mPosition + mDirection;
            if (PACMAN_MAP[nextPosition.y][nextPosition.x] == TILE_EMPTY) {
                mPosition = nextPosition;
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void Player::Draw(void)
{
    mAnimationOffset = mAccumulatedTime < HALF_FRAME ? 0 : 2;
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

    API::Draw(PACMAN_XY(mDirectionOffset, mAnimationOffset), mPosition);
}

///////////////////////////////////////////////////////////////////////////////
Vec2i Player::GetPosition(void) const
{
    return (mPosition);
}

///////////////////////////////////////////////////////////////////////////////
Vec2i Player::GetDirection(void) const
{
    return (mDirection);
}

} // namespace Arc
