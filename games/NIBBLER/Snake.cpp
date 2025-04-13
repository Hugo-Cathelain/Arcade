///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "games/NIBBLER/Snake.hpp"
#include "games/NIBBLER/Assets.hpp"
#include "games/NIBBLER/Maps/Map.hpp"
#include "Arcade/core/API.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Arc::Nibbler
///////////////////////////////////////////////////////////////////////////////

namespace Arc::Nibbler
{

///////////////////////////////////////////////////////////////////////////////
Snake::Snake(void)
    : mDesiredDirection({1,0})
    , mDirection({1,0})
    , mAnimationOffset(0)
    , mDirectionOffset(0)
    , mMovementPercentage(1.f)
    , mMovementSpeed(10.47f)
    , mMovementAccumulator(0.f)
    , isDead(false)
    , mSnakeColor(Snake_Color::RED)
    , mLevel(0)
    , mLives(3)
{
    Vec2f basePosition(15.5f, 29);

    mSnakeParts.push_back(basePosition);
    mSnakeParts.push_back(Vec2f{basePosition.x - 1, basePosition.y});
    mSnakeParts.push_back(Vec2f{basePosition.x - 2, basePosition.y});
    mSnakeParts.push_back(Vec2f{basePosition.x - 3, basePosition.y});
    mSnakeParts.push_back(Vec2f{basePosition.x - 4, basePosition.y});
    mSnakeParts.push_back(Vec2f{basePosition.x - 5, basePosition.y});
    mSnakeParts.push_back(Vec2f{basePosition.x - 6, basePosition.y});
    mSnakeParts.push_back(Vec2f{basePosition.x - 7, basePosition.y});

    mSegmentDirections.resize(mSnakeParts.size(), mDirection);
}

///////////////////////////////////////////////////////////////////////////////
void Snake::SetMovementPercentage(float percentage)
{
    mMovementPercentage = percentage;
}

///////////////////////////////////////////////////////////////////////////////
void Snake::SetPosition(const Vec2i& position)
{
    mSnakeParts[0] = position;
}

///////////////////////////////////////////////////////////////////////////////
Vec2i Snake::GetPosition(void) const
{
    return (Vec2i(std::round(mSnakeParts[0].x), std::round(mSnakeParts[0].y)));
}

///////////////////////////////////////////////////////////////////////////////
void Snake::SetDesiredDirection(const Vec2i& direction)
{
    if (mDesiredDirection == -mDirection) {
        return;
    }

    // Vec2i headPos = GetPosition();
    // mTurnPoints.push_back({headPos, direction});
    mDesiredDirection = direction;
}

///////////////////////////////////////////////////////////////////////////////
Vec2i Snake::GetDirection(void) const
{
    return (mDirection);
}

int Snake::GetLives(void) const
{
    return (mLives);
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
                sprite = SNAKE_HEAD_RIGHT;
            } else if (mDirection.x == -1) {
                sprite = SNAKE_HEAD_LEFT;
            } else if (mDirection.y == -1) {
                sprite = SNAKE_HEAD_TOP;
            } else {
                sprite = SNAKE_HEAD_BOTTOM;
            }
            auto spriteColor = SPRITES[sprite];
            spriteColor.position.x += snakeColor;
            API::Draw(spriteColor, mSnakeParts[i]);

        } else if (i == mSnakeParts.size() - 1) {
            Vec2f diff = {mSnakeParts[i-1].x - mSnakeParts[i].x,
                          mSnakeParts[i-1].y - mSnakeParts[i].y};

            if (diff.x > 0.8f) sprite = SNAKE_TAIL_LEFT;
            else if (diff.x < -0.5f) sprite = SNAKE_TAIL_RIGHT;
            else if (diff.y > 0.5f) sprite = SNAKE_TAIL_TOP;
            else sprite = SNAKE_TAIL_BOTTOM;

            auto spriteColor = SPRITES[sprite];
            spriteColor.position.x += snakeColor;
            API::Draw(spriteColor, mSnakeParts[i]);

        } else {
            Vec2i prevDiff = {static_cast<int>(std::round(mSnakeParts[i-1].x - mSnakeParts[i].x)),
                              static_cast<int>(std::round(mSnakeParts[i-1].y - mSnakeParts[i].y))};
            Vec2i nextDiff = {static_cast<int>(std::round(mSnakeParts[i].x - mSnakeParts[i+1].x)),
                              static_cast<int>(std::round(mSnakeParts[i].y - mSnakeParts[i+1].y))};

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
}

///////////////////////////////////////////////////////////////////////////////
void Snake::Grow(void)
{
    mSnakeParts.push_back(mSnakeParts[mSnakeParts.size() - 1]);
    mSegmentDirections.push_back(mSegmentDirections.back());
}

///////////////////////////////////////////////////////////////////////////////
void Snake::Update(float deltaSeconds)
{
    (void)deltaSeconds;

    float speed = mMovementSpeed * mMovementPercentage;

    // Calculate the new direction
    Vec2i headPosition = GetPosition();

    if (mDesiredDirection != Vec2i(0)) {
        Vec2i nextPosition = headPosition + mDesiredDirection;

        if (MAPS[mLevel % MAPS.size()][nextPosition.x][nextPosition.y - ARCADE_OFFSET_Y] == EMPTY) {
            mDirection = mDesiredDirection;
            mSnakeParts[0] = Vec2f(headPosition);
            // mSegmentDirections[0] = mDirection;
            // mTurnPoints.push_back({headPosition, mDirection});
            mDesiredDirection = Vec2i(0);
        }
    }

    Vec2i next = headPosition + mDirection;

    if (MAPS[mLevel % MAPS.size()][next.x][next.y - ARCADE_OFFSET_Y] != EMPTY) {
        std::vector<Vec2i> directions = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
        std::vector<Vec2i> validDirections;

        for (const auto& dir : directions) {
            Vec2i nextPosition = headPosition + dir;

            if (dir == mDirection || dir == -mDirection) {
                continue;
            }

            if (MAPS[mLevel % MAPS.size()][nextPosition.x][nextPosition.y - ARCADE_OFFSET_Y] == EMPTY) {
                validDirections.push_back(dir);
            }
        }

        if (validDirections.size() != 1) {
            speed = 0;
        } else {
            mDesiredDirection = validDirections[0];
            // mTurnPoints.push_back({headPosition, mDirection});
        }
    }

// for (size_t i = 0; i < mSnakeParts.size(); ++i) {
//     Vec2f& segment = mSnakeParts[i];

//     // HEAD
//     if (i == 0) {
//         Vec2i currentTile = GetPosition();
//         std::cout << "[HEAD] Current tile: (" << currentTile.x << ", " << currentTile.y << ")\n";

//         if (!mTurnPoints.empty()) {
//             std::cout << "[TURN] Next TurnPoint at: (" << mTurnPoints.front().position.x << ", "
//                       << mTurnPoints.front().position.y << ") Dir: ("
//                       << mTurnPoints.front().direction.x << ","
//                       << mTurnPoints.front().direction.y << ")\n";
//         }

//         //Check if the head reached a turn point
//         if (!mTurnPoints.empty()&& currentTile == mTurnPoints.front().position) {
//             mDirection = mTurnPoints.front().direction;
//             mSegmentDirections[0] = mDirection;
//             std::cout << "[HEAD TURN] --------------------------New direction: (" << mDirection.x << "," << mDirection.y << ")\n";
//             mTurnPoints.pop_front();
//         }

//         segment += Vec2f(mSegmentDirections[0]) * speed * deltaSeconds;

//         Vec2i target = currentTile + mSegmentDirections[0];
//         Vec2f diff = Vec2f(target) - segment;
//         if (std::sqrt(diff.x * diff.x + diff.y * diff.y) < 0.1f) {
//             segment = Vec2f(target);
//             std::cout << "[HEAD SNAP] --------------------------Snapped to: (" << target.x << ", " << target.y << ")\n";
//         }
//     }

//     // BODY SEGMENTS
//     else {
//         Vec2i currentTile = Vec2i(std::round(segment.x), std::round(segment.y));
//         std::cout << "[SEGMENT " << i << "] Current tile: (" << currentTile.x << ", " << currentTile.y << ")\n";

//         for (const TurnPoint& tp : mTurnPoints) {
//             std::cout << "------------------------------------------[SEGMENT " << i << "] Checking turn point: (" << tp.position.x << ", " << tp.position.y << ")\n";
//             if (currentTile == tp.position) {
//                 mSegmentDirections[i] = tp.direction;
//                 std::cout << "[SEGMENT " << i << " TURN] Changed direction to: ("
//                           << tp.direction.x << "," << tp.direction.y << ")\n";
//                 break;
//             }
//         }

//         segment += Vec2f(mSegmentDirections[i]) * speed * deltaSeconds;

//         Vec2i target = currentTile + mSegmentDirections[i];
//         Vec2f diff = Vec2f(target) - segment;
//         if (std::sqrt(diff.x * diff.x + diff.y * diff.y) < 0.1f) {
//             segment = Vec2f(target);
//             std::cout << "[SEGMENT " << i << " SNAP] Snapped to: (" << target.x << ", " << target.y << ")\n";
//         }
//     }

//     std::cout << "[DEBUG] Segment[" << i << "] pos = (" << segment.x << ", " << segment.y
//               << ") dir = (" << mSegmentDirections[i].x << ", " << mSegmentDirections[i].y << ")\n";
// }


//     if (!mTurnPoints.empty()) {
//         Vec2i tailTile = Vec2i(std::round(mSnakeParts.back().x), std::round(mSnakeParts.back().y));
//         if (tailTile == mTurnPoints.front().position) {
//             mTurnPoints.pop_front();
//         }
//     }


    Vec2f prevPart = mSnakeParts[0];

    Vec2f previous;

    for (int i = 0; i < (int)mSnakeParts.size(); i++) {
        if (i == 0) {
            previous = mSnakeParts[i];
            mSnakeParts[i] += Vec2f(mDirection) * speed * deltaSeconds;

            // Snap head to grid when close enough to next cell
            Vec2i targetPos = GetPosition() + mDirection;
            Vec2f diff = Vec2f(targetPos) - mSnakeParts[i];
            float distance = std::sqrt(diff.x * diff.x + diff.y * diff.y);
            if (distance < 0.1f) {
                mSnakeParts[i] = Vec2f(targetPos);
            }
        } else {
            Vec2f targetPosition = previous;
            Vec2f direction = targetPosition - mSnakeParts[i];
            float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

            // Maintain fixed distance between snake parts
            float desiredDistance = 0.8f;
            if (distance > desiredDistance) {
                direction = direction / distance; // Normalize
                Vec2f temp = mSnakeParts[i];
                mSnakeParts[i] += direction * (distance - desiredDistance);
                previous = temp;
            } else {
                previous = mSnakeParts[i];
            }
        }

        prevPart = mSnakeParts[i];
    }
}

void Snake::Reset(void)
{
    mSnakeParts.clear();
    Vec2f basePosition(15.5f, 29);

    mSnakeParts.push_back(basePosition);
    mSnakeParts.push_back(Vec2f{basePosition.x - 1, basePosition.y});
    mSnakeParts.push_back(Vec2f{basePosition.x - 2, basePosition.y});
    mSnakeParts.push_back(Vec2f{basePosition.x - 3, basePosition.y});
    mSnakeParts.push_back(Vec2f{basePosition.x - 4, basePosition.y});
    mSnakeParts.push_back(Vec2f{basePosition.x - 5, basePosition.y});
    mSnakeParts.push_back(Vec2f{basePosition.x - 6, basePosition.y});
    mSnakeParts.push_back(Vec2f{basePosition.x - 7, basePosition.y});
    mSegmentDirections.resize(mSnakeParts.size(), mDirection);
    mDirection = {1, 0};
    mDesiredDirection = {1, 0};
    mAnimationOffset = 0;
    mDirectionOffset = 0;
    mMovementPercentage = 1.f;
    mMovementSpeed = 10.47f;
    mMovementAccumulator = 0.f;
}

} // namespace Arc::Nibbler
