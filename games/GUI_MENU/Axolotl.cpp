///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "games/GUI_MENU/Axolotl.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Arc
///////////////////////////////////////////////////////////////////////////////
namespace Arc
{

///////////////////////////////////////////////////////////////////////////////
Axolotl::Axolotl(void)
    : mState(State::DEAD)
    , mLastTimer(0.f)
    , mStartTime(0.f)
    , mCurrentFrame(0)
    , mSoundTimer(0.f)
{}

///////////////////////////////////////////////////////////////////////////////
void Axolotl::ChangeState(State state)
{
    if (state == mState || (mState == State::DEAD && state != State::ARISE)) {
        return;
    }

    if (state == State::DANCE) {
        mSoundTimer = 0.f;
        API::PlaySound("assets/GUI/sfx/blingblangblang.wav");
    }

    mStartTime = mLastTimer;
    mState = state;
    mCurrentFrame = 0;
}

///////////////////////////////////////////////////////////////////////////////
void Axolotl::Draw(float timer)
{
    mSoundTimer += timer - mLastTimer;
    mLastTimer = timer;
    float deltaT = timer - mStartTime;
    int frame = 0;
    int offsetY = 0;

    switch (mState) {
        case State::IDLE:
            offsetY = AXOLOTL_IDLE_OFFSET_Y;
            frame = static_cast<int>(deltaT * 6) % AXOLOTL_IDLE_FRAME;
            break;
        case State::ARISE:
            offsetY = AXOLOTL_ARISE_OFFSET_Y;
            frame = static_cast<int>(deltaT * 6) % AXOLOTL_ARISE_FRAME;
            if (frame == 0 && deltaT > 0.5f) {
                mState = State::IDLE;
            }
            break;
        case State::DANCE:
            offsetY = AXOLOTL_DANCE_OFFSET_Y;
            frame = static_cast<int>(deltaT * 6) % AXOLOTL_DANCE_FRAME;

            if (mSoundTimer > 8.5f) {
                mState = State::IDLE;
            }
            break;
        case State::DEAD:
            offsetY = AXOLOTL_ARISE_OFFSET_Y;
            frame = 0;
            break;
        case State::DIE:
            offsetY = AXOLOTL_DIE_OFFSET_Y;
            frame = static_cast<int>(deltaT * 6) % AXOLOTL_DIE_FRAME;
            if (frame == 0 && deltaT > 0.5f) {
                mState = State::DEAD;
            }
            break;
        case State::EAT:
            offsetY = AXOLOTL_EAT_OFFSET_Y;
            frame = static_cast<int>(deltaT * 6) % AXOLOTL_EAT_FRAME;
            if (frame == 0 && deltaT > 0.5f) {
                mState = State::IDLE;
            }
            break;
    }

    API::Draw(
        IGameModule::Asset(
            {AXOLOTL_OFFSET_X + frame * 3, offsetY},
            "PET", Color{255, 255, 255},
            {24, 24}
        ),
        Vec2i{1, 33}
    );
}

} // namespace Arc
