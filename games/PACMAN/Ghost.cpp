///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "games/PACMAN/Ghost.hpp"
#include "games/PACMAN/Assets.hpp"
#include "games/PACMAN/Random.hpp"
#include "games/PACMAN/Player.hpp"
#include "../../Arcade/core/API.hpp"
#include <algorithm>

///////////////////////////////////////////////////////////////////////////////
// Namespace Arc::Pacman
///////////////////////////////////////////////////////////////////////////////
namespace Arc::Pacman
{


///////////////////////////////////////////////////////////////////////////////
const Vec2i Ghost::CORNER_TARGETS[4] = {
    Vec2i(ARCADE_GAME_WIDTH - 3, -4),                       // BLINKY
    Vec2i(2, -4),                                           // PINKY
    Vec2i(0, ARCADE_SCREEN_HEIGHT),                         // INKY
    Vec2i(ARCADE_GAME_WIDTH - 1, ARCADE_SCREEN_HEIGHT)      // CLYDE
};

///////////////////////////////////////////////////////////////////////////////
Ghost::Ghost(Type type)
    : mType(type)
    , mState(State::SCATTER)
    , mPosition(static_cast<float>(type) * 2.f + 9.5f, 14.f)
    , mDirection(0, 0)
    , mMovementPercentage(1.f)
    , mMovementSpeed(10.f)
    , mMovementAccumulator(0.f)
    , mInGhostHouse(true)
    , mAccumulator(0.f)
{
    if (type == Type::BLINKY) {
        mInGhostHouse = false;
        mState = State::CHASE;
        mPosition = Vec2f(13.5f, 11.f);
    }
}

///////////////////////////////////////////////////////////////////////////////
Ghost::Type Ghost::GetType(void) const
{
    return (mType);
}

///////////////////////////////////////////////////////////////////////////////
Vec2i Ghost::GetPosition(void) const
{
    return (mPosition);
}

///////////////////////////////////////////////////////////////////////////////
void Ghost::SetMovementPercentage(float percentage)
{
    mMovementPercentage = percentage;
}

///////////////////////////////////////////////////////////////////////////////
void Ghost::SetPosition(const Vec2i& position)
{
    mPosition = position;
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
void Ghost::CalculateBlinkyTarget(std::unique_ptr<Player>& pacman)
{
    mTarget = pacman->GetPosition();
}

///////////////////////////////////////////////////////////////////////////////
void Ghost::CalculatePinkyTarget(std::unique_ptr<Player>& pacman)
{
    Vec2i pacmanPos = pacman->GetPosition();
    Vec2i pacmanDir = pacman->GetDirection();

    if (pacmanDir.y == -1) {
        mTarget = Vec2i(pacmanPos.x - 4, pacmanPos.y - 4);
    } else {
        mTarget = Vec2i(
            pacmanPos.x + pacmanDir.x * 4,
            pacmanPos.y + pacmanDir.y * 4
        );
    }
}

///////////////////////////////////////////////////////////////////////////////
void Ghost::CalculateInkyTarget(
    std::unique_ptr<Player>& pacman,
    const Vec2i& blinkyPos
)
{
    Vec2i pacmanPos = pacman->GetPosition();
    Vec2i pacmanDir = pacman->GetDirection();
    Vec2i intermediate;

    if (pacmanDir.y == -1) {
        intermediate = Vec2i(pacmanPos.x - 2, pacmanPos.y - 2);
    } else {
        intermediate = Vec2i(
            pacmanPos.x + pacmanDir.x * 2,
            pacmanPos.y + pacmanDir.y * 2
        );
    }

    Vec2i vector = Vec2i(
        intermediate.x - blinkyPos.x,
        intermediate.y - blinkyPos.y
    );
    mTarget = Vec2i(intermediate.x + vector.x, intermediate.y + vector.y);
}

///////////////////////////////////////////////////////////////////////////////
void Ghost::CalculateClydeTarget(std::unique_ptr<Player>& pacman)
{
    Vec2i pacmanPos = pacman->GetPosition();
    Vec2i ghostPos = Vec2i(mPosition);

    float distance = (Vec2f(ghostPos) - Vec2f(pacmanPos)).Length();

    if (distance >= 8.0f) {
        mTarget = pacmanPos;
    } else {
        mTarget = CORNER_TARGETS[static_cast<int>(Type::CLYDE)];
    }
}

///////////////////////////////////////////////////////////////////////////////
void Ghost::CalculateFrightenedDirection(void)
{
    static const std::vector<Vec2i> PRIORITISED_DIRECTION = {
        Vec2i(0, -1), Vec2i(-1, 0), Vec2i(0, 1), Vec2i(1, 0)
    };

    Vec2i forbidden = -mDirection;
    unsigned int index = RNG::Get() % 4;

    while (true) {
        Vec2i dir = PRIORITISED_DIRECTION[index];
        Vec2i next = Vec2i(mPosition + Vec2f(dir));

        if (dir != forbidden && PACMAN_MAP[next.y][next.x] == TILE_EMPTY) {
            mDirection = dir;
            return;
        }

        index = (index + 1) % 4;
    }
}

///////////////////////////////////////////////////////////////////////////////
void Ghost::HandleTunnelPassage(void)
{
    if (static_cast<int>(mPosition.x) <= 0) {
        mPosition.x = ARCADE_GAME_WIDTH - 1;
    } else if (static_cast<int>(mPosition.x) >= ARCADE_GAME_WIDTH - 1) {
        mPosition.x = 0;
    }
}

///////////////////////////////////////////////////////////////////////////////
void Ghost::CalculateBestDirection(void)
{
    static const std::vector<Vec2i> PRIORITISED_DIRECTION = {
        Vec2i(0, -1), Vec2i(-1, 0), Vec2i(0, 1), Vec2i(1, 0)
    };

    Vec2i forbidden = -mDirection;
    std::map<float, Vec2i> directions;

    for (const auto& dir : PRIORITISED_DIRECTION) {
        int x = static_cast<int>(mPosition.x + dir.x);
        int y = static_cast<int>(mPosition.y + dir.y);

        if (dir == forbidden || PACMAN_MAP[y][x] != TILE_EMPTY) {
            continue;
        }

        float distance = (Vec2f(x, y) - Vec2f(mTarget)).Length();
        directions[distance] = dir;
    }

    if (directions.empty()) {
        for (const auto& dir : PRIORITISED_DIRECTION) {
            int x = static_cast<int>(mPosition.x + dir.x);
            int y = static_cast<int>(mPosition.y + dir.y);

            if (PACMAN_MAP[y][x] == TILE_EMPTY) {
                mDirection = dir;
                return;
            }
        }
        return;
    }

    mDirection = directions.begin()->second;
}

///////////////////////////////////////////////////////////////////////////////
void Ghost::Update(
    float deltaSeconds,
    std::unique_ptr<Player>& pacman,
    const Vec2i& blinkyPos
)
{
    if (mInGhostHouse) {
        return;
    }

    float speed = deltaSeconds * mMovementSpeed * mMovementPercentage;

    switch (mState) {
        case State::CHASE:
            if (mType == Type::BLINKY) {
                CalculateBlinkyTarget(pacman);
            } else if (mType == Type::PINKY) {
                CalculatePinkyTarget(pacman);
            } else if (mType == Type::INKY) {
                CalculateInkyTarget(pacman, blinkyPos);
            } else if (mType == Type::CLYDE) {
                CalculateClydeTarget(pacman);
            }
            CalculateBestDirection();
            mPosition += Vec2f(mDirection) * speed;
            break;
        case State::SCATTER:
            mTarget = CORNER_TARGETS[static_cast<int>(mType)];
            CalculateBestDirection();
            mPosition += Vec2f(mDirection) * speed;
            break;
        case State::EATEN:
            mTarget = Vec2i(13, 11);
            CalculateBestDirection();
            mPosition += Vec2f(mDirection) * speed;
            break;
        case State::FRIGHTENED:
            CalculateFrightenedDirection();
            mPosition += Vec2f(mDirection) * speed;
            break;
    }

    HandleTunnelPassage();
}

///////////////////////////////////////////////////////////////////////////////
void Ghost::IncrementCounter(void)
{
    mCounter++;
}

///////////////////////////////////////////////////////////////////////////////
void Ghost::ResetCounter(void)
{
    mCounter = 0;
}

///////////////////////////////////////////////////////////////////////////////
void Ghost::Draw(float timer)
{
    static const Color GHOST_COLORS[] = {
        CLR_RED, CLR_PINK, CLR_CYAN, CLR_ORANGE
    };

    int index = static_cast<int>(mType);
    int flickering = static_cast<int>(timer * 8) % 2 ? 2 : 0;
    int directionOffset = 0;

    if (mDirection.x == 1) {
        directionOffset = 0;
    } else if (mDirection.x == -1) {
        directionOffset = 4;
    } else if (mDirection.y == -1) {
        directionOffset = 8;
    } else if (mDirection.y == 1) {
        directionOffset = 12;
    }

    Vec2f position = mPosition + Vec2f(0, ARCADE_OFFSET_Y);

    switch (mState) {
        case State::CHASE:
        case State::SCATTER:
            API::Draw(
                IGameModule::Asset(
                    {12 + directionOffset + flickering, index * 2},
                    "/\\", GHOST_COLORS[index], {16, 16}
                ),
                position
            );
            break;
        case State::EATEN:
            API::Draw(
                IGameModule::Asset(
                    {20 + directionOffset / 2, 8},
                    "oo", CLR_WHITE, {16, 16}
                ),
                position
            );
            break;
        case State::FRIGHTENED:
            IGameModule::Asset scared = SPRITES[SCARED_1 + flickering / 2];
            scared.id = -1;
            API::Draw(scared, position);
            break;
    }
}

} // namespace Arc::Pacman
