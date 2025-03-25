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
    , mMovementSpeed(7.f)
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
    if (mAccumulatedTime >= 0.5f) {
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
    int animationOffset = mAccumulatedTime < 0.25f ? 0 : 2;
    int directionOffset = 0;

    if (mDirection == Vec2i(0, 0)) {
        directionOffset = 0;
        animationOffset = 4;
    } else if (mDirection.x == 1) {
        directionOffset = 0;
    } else if (mDirection.x == -1) {
        directionOffset = 2;
    } else if (mDirection.y == -1) {
        directionOffset = 4;
    } else if (mDirection.y == 1) {
        directionOffset = 6;
    }

    API::Draw(PACMAN_XY(directionOffset, animationOffset), mPosition);
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
