///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "games/PACMAN/Ghost.hpp"
#include "games/PACMAN/Assets.hpp"
#include "../../Arcade/core/API.hpp"
#include <algorithm>

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
    , mMovementSpeed(8.f)
    , mMovementAccumulator(0.f)
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
Vec2i Ghost::GetNextPosition(Vec2i pacmanPos)
{
    Vec2i currentPos = Vec2i(mPosition);

    const std::vector<Vec2i> directions = {
        Vec2i(1, 0),
        Vec2i(-1, 0),
        Vec2i(0, -1),
        Vec2i(0, 1)
    };

    Vec2i oppositeDir(-mDirection.x, -mDirection.y);

    std::vector<std::pair<Vec2i, float>> validMoves;

    for (const auto& dir : directions) {
        if (dir == oppositeDir && mState != State::FRIGHTENED) {
            continue;
        }

        Vec2i nextPos = currentPos + dir;

        if (nextPos.x < 0) {
            nextPos.x = ARCADE_GAME_WIDTH - 1;
        } else if (nextPos.x >= ARCADE_GAME_WIDTH) {
            nextPos.x = 0;
        }

        if (nextPos.y >= 0 && nextPos.y < ARCADE_GAME_HEIGHT &&
            PACMAN_MAP[nextPos.y][nextPos.x] == TILE_EMPTY) {

            float distance = std::abs(nextPos.x - pacmanPos.x) +
                             std::abs(nextPos.y - pacmanPos.y);

            if (mState == State::FRIGHTENED) {
                distance = -distance;
            }

            float randomFactor = 1.0f + (static_cast<float>(rand() % 10) / 100.0f);
            distance *= randomFactor;

            validMoves.push_back({nextPos, distance});
        }
    }

    if (!validMoves.empty()) {
        std::sort(validMoves.begin(), validMoves.end(),
            [](const auto& a, const auto& b) {
                return a.second < b.second;
            });

        Vec2i bestPos = validMoves[0].first;
        mDirection = Vec2i(bestPos.x - currentPos.x, bestPos.y - currentPos.y);

        if (std::abs(mDirection.x) > 1) {
            mDirection.x = (mDirection.x > 0) ? -1 : 1;
        }

        return (bestPos);
    }

    Vec2i backupPos = currentPos + oppositeDir;

    if (backupPos.x < 0) {
        backupPos.x = ARCADE_GAME_WIDTH - 1;
    } else if (backupPos.x >= ARCADE_GAME_WIDTH) {
        backupPos.x = 0;
    }

    if (backupPos.y >= 0 && backupPos.y < ARCADE_GAME_HEIGHT &&
        PACMAN_MAP[backupPos.y][backupPos.x] == TILE_EMPTY) {
        mDirection = oppositeDir;
        return (backupPos);
    }

    return (currentPos);
}

///////////////////////////////////////////////////////////////////////////////
void Ghost::Update(float deltaSeconds, Vec2i pacmanPos)
{
    if (mState == State::IDLING) {
        return;
    }

    float speed = mMovementSpeed;

    if (mState == State::FRIGHTENED) {
        speed = 3.75f;
    }

    float moveThreshold = 1.0f / speed;

    mMovementAccumulator += deltaSeconds;

    if (mMovementAccumulator < moveThreshold) {
        return;
    }
    mMovementAccumulator -= moveThreshold;
    mPosition = GetNextPosition(pacmanPos);;
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
    } else if (mDirection.y == -1) {
        directionOffset = 8;
    } else if (mDirection.y == 1) {
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
