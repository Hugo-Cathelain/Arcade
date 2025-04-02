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
    , mDirection(0, 0)
    , mMovementPercentage(1.f)
    , mMovementSpeed(9.47f)
    , mMovementAccumulator(0.f)
    , mInGhostHouse(true)
    , mAccumulator(0.f)
    , mDirectionChange(false)
{
    if (type == Type::PINKY) {
        mPosition = Vec2f(13.5f, 14.f);
    } else if (type == Type::INKY) {
        mPosition = Vec2f(11.5f, 14.f);
    } else if (type == Type::CLYDE) {
        mPosition = Vec2f(15.5f, 14.f);
    } else if (type == Type::BLINKY) {
        mInGhostHouse = false;
        mState = State::CHASE;
        mPosition = Vec2f(13.5f, 11.f);
    }

    mNextTile = Vec2i(
        static_cast<int>(std::floor(mPosition.x + 0.5f)),
        static_cast<int>(std::floor(mPosition.y + 0.5f))
    );
}

///////////////////////////////////////////////////////////////////////////////
Ghost::Type Ghost::GetType(void) const
{
    return (mType);
}

///////////////////////////////////////////////////////////////////////////////
Vec2i Ghost::GetPosition(void) const
{
    return (Vec2i(
        static_cast<int>(std::floor(mPosition.x + 0.5f)),
        static_cast<int>(std::floor(mPosition.y + 0.5f))
    ));
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
    if ((state == State::CHASE && mState == State::SCATTER) ||
        (state == State::SCATTER && mState == State::CHASE)
    ) {
        mDirectionChange = true;
    }

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

    Vec2i currentTile(
        static_cast<int>(std::floor(mPosition.x + 0.5f)),
        static_cast<int>(std::floor(mPosition.y + 0.5f))
    );

    Vec2i direction = mDirection;
    Vec2i forbidden = -mDirection;
    unsigned int index = RNG::Get() % 4;

    while (true) {
        Vec2i dir = PRIORITISED_DIRECTION[index];
        Vec2i next = Vec2i(mPosition + Vec2f(dir));

        if (next.x < 0) next.x = ARCADE_GAME_WIDTH - 1;
        else if (next.x > (ARCADE_GAME_WIDTH - 1)) next.x = 0;

        if (dir != forbidden && PACMAN_MAP[next.y][next.x] == TILE_EMPTY) {
            direction = dir;
            break;
        }

        index = (index + 1) % 4;
    }

    mTarget = Vec2i(currentTile + direction);
}

///////////////////////////////////////////////////////////////////////////////
void Ghost::HandleTunnelPassage(void)
{
    if (static_cast<int>(mPosition.x) < 0) {
        mPosition.x = ARCADE_GAME_WIDTH - 1;
    } else if (static_cast<int>(mPosition.x) > ARCADE_GAME_WIDTH - 1) {
        mPosition.x = 0;
    }
}

///////////////////////////////////////////////////////////////////////////////
void Ghost::CalculateBestDirection(void)
{
    static const std::vector<Vec2i> PRIORITISED_DIRECTION = {
        Vec2i(0, -1), Vec2i(-1, 0), Vec2i(0, 1), Vec2i(1, 0)
    };

    Vec2i currentTile(
        static_cast<int>(std::floor(mPosition.x + 0.5f)),
        static_cast<int>(std::floor(mPosition.y + 0.5f))
    );

    Vec2i forbidden = -mDirection;
    float minDistance = std::numeric_limits<float>::max();
    Vec2i direction(0);

    for (const auto& dir : PRIORITISED_DIRECTION) {
        Vec2i nextTile = currentTile + dir;

        if (nextTile.x < 0) nextTile.x = ARCADE_GAME_WIDTH - 1;
        else if (nextTile.x > (ARCADE_GAME_WIDTH - 1)) nextTile.x = 0;

        if (dir == forbidden || PACMAN_MAP[nextTile.y][nextTile.x] != TILE_EMPTY) {
            continue;
        }

        if (dir == Vec2i(0, -1) && currentTile.y == 11 && currentTile.x >= 10 && currentTile.x <= 19) {
            continue;
        }

        float distance = (Vec2f(nextTile) - Vec2f(mTarget)).Length();

        if (distance < minDistance) {
            minDistance = distance;
            direction = dir;
        } else if (distance == minDistance) {
            size_t currentIndex = 0;
            size_t newIndex = 0;

            for (size_t i = 0; i < PRIORITISED_DIRECTION.size(); i++) {
                if (PRIORITISED_DIRECTION[i] == mDirection) {
                    currentIndex = i;
                }
                if (PRIORITISED_DIRECTION[i] == dir) {
                    newIndex = i;
                }
            }

            if (newIndex < currentIndex) {
                direction = dir;
            }
        }
    }

    if (direction != mDirection && mNextTile == currentTile) {
        float length = (mPosition - Vec2f(currentTile)).Length() - 0.5f;

        if (length > 0.001f) {
            return;
        }

        if (direction.x != 0) {
            mPosition.y = static_cast<float>(currentTile.y);
        } else if (direction.y != 0) {
            mPosition.x = static_cast<float>(currentTile.x);
        }

        mDirection = direction;
        mNextTile = currentTile + direction;
    } else if (mNextTile == currentTile) {
        mNextTile = currentTile + direction;
    }
}

///////////////////////////////////////////////////////////////////////////////
void Ghost::HandleGhostHouseExit(float speed)
{
    Vec2f center = Vec2f(13.5f, 0.f) - mPosition;

    if (center.x != 0.f) {
        mDirection = Vec2i(center.x > 0.f ? 1 : -1, 0);
        mPosition += Vec2f(mDirection) * speed / 2.f;
        if (mPosition.x > 13.25f && mPosition.x < 13.75f) {
            mPosition.x = 13.5f;
        }
    } else {
        mDirection = Vec2i(0, -1);
        mPosition += Vec2f(mDirection) * speed / 2.f;
        if (mPosition.y < 11.25f) {
            mPosition.y = 11.f;
            mNextTile = Vec2i(
                static_cast<int>(std::floor(mPosition.x + 0.5f)),
                static_cast<int>(std::floor(mPosition.y + 0.5f))
            );
            mInGhostHouse = false;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void Ghost::HandleGhostInHouse(float speed)
{
    if ((mType == Type::PINKY || mType == Type::BLINKY) && mCounter >= 0) {
        HandleGhostHouseExit(speed);
        return;
    } else if (mType == Type::INKY && mCounter >= 30) {
        HandleGhostHouseExit(speed);
        return;
    } else if (mType == Type::CLYDE && mCounter >= 60) {
        HandleGhostHouseExit(speed);
        return;
    }

    if (mDirection.y == 0 && mType != Type::PINKY) {
        mDirection = Vec2i(0, -1);
    } else if (mDirection.y == 0) {
        mDirection = Vec2i(0, 1);
    }

    Vec2f newPosition = mPosition + Vec2f(mDirection) * speed * 0.5f;

    const float UPPER_BOUND = 13.0f;
    const float LOWER_BOUND = 14.5f;

    if (newPosition.y < UPPER_BOUND && mDirection.y < 0) {
        mDirection.y = 1;
        newPosition.y = UPPER_BOUND;
    } else if (newPosition.y > LOWER_BOUND && mDirection.y > 0) {
        mDirection.y = -1;
        newPosition.y = LOWER_BOUND;
    }

    mPosition = newPosition;
}

///////////////////////////////////////////////////////////////////////////////
void Ghost::HandleGhostReturnToHouse(float speed)
{
    Vec2f center = Vec2f(13.5f, 11.f) - mPosition;

    if (center.x != 0.f) {
        mDirection = Vec2i(center.x > 0.f ? 1 : -1, 0);
        mPosition += Vec2f(mDirection) * speed;
        if (mPosition.x > 13.25f && mPosition.x < 13.75f) {
            mPosition.x = 13.5f;
        }
    } else {
        mDirection = Vec2i(0, 1);
        mPosition += Vec2f(mDirection) * speed;
        if (mPosition.y > 14.25f) {
            mPosition.y = 14.f;
            mNextTile = Vec2i(
                static_cast<int>(std::floor(mPosition.x + 0.5f)),
                static_cast<int>(std::floor(mPosition.y + 0.5f))
            );
            mState = State::SCATTER;
            mInGhostHouse = true;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void Ghost::HandleGhostDirectionChange(void)
{
    Vec2i currentTile = GetPosition();
    Vec2i nextTile = currentTile + mDirection;

    if (nextTile.x < 0) nextTile.x = ARCADE_GAME_WIDTH - 1;
    else if (nextTile.x > (ARCADE_GAME_WIDTH - 1)) nextTile.x = 0;

    if (PACMAN_MAP[nextTile.y][nextTile.x] != TILE_EMPTY) {
        return;
    }

    mDirection = -mDirection;
    mNextTile = Vec2i(
        static_cast<int>(std::floor(mPosition.x + 0.5f)),
        static_cast<int>(std::floor(mPosition.y + 0.5f))
    );
    mDirectionChange = false;
}

///////////////////////////////////////////////////////////////////////////////
void Ghost::Update(
    float deltaSeconds,
    std::unique_ptr<Player>& pacman,
    const Vec2i& blinkyPos
)
{
    float speed = deltaSeconds * mMovementSpeed * mMovementPercentage;

    if (mState == State::EATEN && mPosition.x >= 13.0f &&
        mPosition.x <= 14.0f && mPosition.y >= 11.0f && mPosition.y <= 14.5f) {
        HandleGhostReturnToHouse(speed);
        return;
    }

    if (mInGhostHouse) {
        HandleGhostInHouse(speed);
        return;
    }

    if (mDirectionChange) {
        HandleGhostDirectionChange();
    }

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
            break;
        case State::SCATTER:
            mTarget = CORNER_TARGETS[static_cast<int>(mType)];
            break;
        case State::EATEN:
            mTarget = Vec2i(13, 11);
            break;
        case State::FRIGHTENED:
            CalculateFrightenedDirection();
            break;
    }

    CalculateBestDirection();
    mPosition += Vec2f(mDirection) * speed;
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
