///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "games/PACMAN/Ghost.hpp"
#include "games/PACMAN/Assets.hpp"
#include "../../Arcade/core/API.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Arc::Pacman
///////////////////////////////////////////////////////////////////////////////
namespace Arc::Pacman
{

///////////////////////////////////////////////////////////////////////////////
Ghost::Ghost(Type type)
    : mType(type)
    , mState(State::IDLING)
    , mPosition(static_cast<float>(type) * 2.f + 9.5f, 14.f)
    , mDirection(0, 0)
{
    if (type == Type::BLINKY) {
        mState = State::CHASING;
        mPosition = Vec2f(13.5f, 11.f);
    }
}

///////////////////////////////////////////////////////////////////////////////
Ghost::State Ghost::GetState(void) const
{
    return (mState);
}

///////////////////////////////////////////////////////////////////////////////
void Ghost::SetState(Ghost::State state)
{
    mState = state;
}

///////////////////////////////////////////////////////////////////////////////
void Ghost::Update(float deltaSeconds)
{
    (void)deltaSeconds;
}

///////////////////////////////////////////////////////////////////////////////
void Ghost::Draw(float timer)
{
    static const Color GHOST_COLORS[] = {
        CLR_RED,
        CLR_PINK,
        CLR_CYAN,
        CLR_ORANGE
    };

    int index = static_cast<int>(mType);
    int flickering = static_cast<int>(timer * 8) % 2 ? 2 : 0;
    int directionOffset = 0;

    if (mDirection.x == 1) {
        directionOffset = 0;
    } else if (mDirection.x == -1) {
        directionOffset = 4;
    } else if (mDirection.y == 1) {
        directionOffset = 8;
    } else if (mDirection.y == -1) {
        directionOffset = 12;
    }

    if (mState == State::FRIGHTENED) {
        auto scared = SPRITES[SCARED_1 + flickering / 2];
        scared.id = index * 4;

        API::Draw(
            scared,
            mPosition + Vec2f(0, ARCADE_OFFSET_Y)
        );
    } else if (mState == State::KILLED) {

    } else {
        API::Draw(
            IGameModule::Asset(
                {12 + directionOffset + flickering, index * 2},
                "/\\", GHOST_COLORS[index], {16, 16},
                index * 4
            ),
            mPosition + Vec2f(0, ARCADE_OFFSET_Y)
        );
    }
}

} // namespace Arc::Pacman
