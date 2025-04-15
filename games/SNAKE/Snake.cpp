///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "games/SNAKE/Snake.hpp"
#include "Arcade/core/API.hpp"
#include "games/SNAKE/Assets.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>

///////////////////////////////////////////////////////////////////////////////
// Namespace Arc
///////////////////////////////////////////////////////////////////////////////
namespace Arc
{

///////////////////////////////////////////////////////////////////////////////
Snake::Snake(void)
    : mGameOver(false)
    , mAccumulatedTime(0.0f)
    , mOffset({1, 0})
    , mNewOffset({1, 0})
    , mPosition({16, 14})
    , mScore(0)
    , mIngame(false)
{
    mBestScore = 0;

    mSnakeParts.push_back(mPosition);
    mSnakeParts.push_back(Vec2i{mPosition.x - 2, mPosition.y});
    mSnakeParts.push_back(Vec2i{mPosition.x - 4, mPosition.y});
    mSnakeParts.push_back(Vec2i{mPosition.x - 6, mPosition.y});

    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    respawnApple();
}

///////////////////////////////////////////////////////////////////////////////
Snake::~Snake()
{}

///////////////////////////////////////////////////////////////////////////////
void Snake::respawnApple(void)
{
    int x = (std::rand() % 15) * 2;
    int y = (std::rand() % 12) * 2 + 4;

    for (const auto& part : mSnakeParts) {
        if (part.x == x && part.y == y) {
            respawnApple();
            return;
        }
    }

    mApplePosition = {x, y};
}

///////////////////////////////////////////////////////////////////////////////
void Snake::BeginPlay(void)
{
    API::PushEvent(API::Event::GRAPHICS, API::Event::GridSize({31, 28}));
}

///////////////////////////////////////////////////////////////////////////////
void Snake::EndPlay(void)
{}

///////////////////////////////////////////////////////////////////////////////
void Snake::Text(
    const std::string& text,
    Snake::TextColor color,
    const Vec2i& position
)
{
    static const int LTRS_OFFSET_Y = 8;
    static const char LTRS[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ!@..0123456789/-\"";
    static const int LTRS_ROW_SIZE = 15;
    static const Color LTS_COLORS[] = {
        Color{224, 221, 255}, Color{255, 0, 0}, Color{252, 181, 255},
        Color{0, 255, 255}, Color{248, 187, 85}, Color{250, 185, 176},
        Color{255, 255, 0}
    };

    for (size_t i = 0; i < text.size(); i++) {
        int index = -1;
        for (size_t j = 0; j < sizeof(LTRS); j++) {
            if (text[i] == LTRS[j]) {
                index = j;
                break;
            }
        }

        if (index == -1) {
            continue;
        }

        int row = index / LTRS_ROW_SIZE + static_cast<int>(color) * 4;
        int col = index % LTRS_ROW_SIZE;

        IGameModule::Asset sprite(
            {col, LTRS_OFFSET_Y + row},
            std::string(1, text[i]),
            LTS_COLORS[static_cast<int>(color)]
        );

        API::Draw(sprite, position + Vec2i{static_cast<int>(i), 0});
    }
}

///////////////////////////////////////////////////////////////////////////////
void Snake::handleKeyPressed(EKeyboardKey key)
{
    mNewOffset = mOffset;
    switch (key) {
        case EKeyboardKey::UP:
            mNewOffset = {0, -1};
            break;
        case EKeyboardKey::DOWN:
            mNewOffset = {0, 1};
            break;
        case EKeyboardKey::LEFT:
            mNewOffset = {-1, 0};
            break;
        case EKeyboardKey::RIGHT:
            mNewOffset = {1, 0};
            break;
        case EKeyboardKey::SPACE:
            if (mGameOver || !mIngame) {
                mGameOver = false;
                mIngame = true;
                mScore = 0;
                mSnakeParts.clear();
                mSnakeParts.push_back(mPosition);
                mSnakeParts.push_back(Vec2i{mPosition.x - 2, mPosition.y});
                mSnakeParts.push_back(Vec2i{mPosition.x - 4, mPosition.y});
                mSnakeParts.push_back(Vec2i{mPosition.x - 6, mPosition.y});
                mOffset = {1, 0};
                mNewOffset = {1, 0};
                respawnApple();
            }
            break;
        default:
            break;
    }
}

///////////////////////////////////////////////////////////////////////////////
void Snake::drawSpritesSnake(void)
{
    // Draw snake parts
    for (size_t i = 0; i < mSnakeParts.size(); i++) {
        SpriteType sprite;

        if (i == 0) {
            Vec2i currentDirection;
            if (mSnakeParts.size() > 1) {
                currentDirection = {
                    mSnakeParts[0].x - mSnakeParts[1].x,
                    mSnakeParts[0].y - mSnakeParts[1].y
                };
            } else {
                currentDirection = mOffset;
            }

            if (std::abs(currentDirection.x) > 15) {
                currentDirection.x = currentDirection.x > 0 ? -1 : 1;
            }
            if (std::abs(currentDirection.y) > 12) {
                currentDirection.y = currentDirection.y > 0 ? -1 : 1;
            }

            if (currentDirection.x > 0) {
                sprite = SNAKE_HEAD_L;
            } else if (currentDirection.x < 0) {
                sprite = SNAKE_HEAD_R;
            } else if (currentDirection.y > 0) {
                sprite = SNAKE_HEAD_B;
            } else {
                sprite = SNAKE_HEAD_T;
            }
        } else if (i == mSnakeParts.size() - 1) {
            Vec2i diff = {
                mSnakeParts[i-1].x - mSnakeParts[i].x,
                mSnakeParts[i-1].y - mSnakeParts[i].y
            };

            if (std::abs(diff.x) > 15) {
                diff.x = diff.x > 0 ? -1 : 1;
            }
            if (std::abs(diff.y) > 12) {
                diff.y = diff.y > 0 ? -1 : 1;
            }

            if (diff.x > 0) {
                sprite = SNAKE_TAIL_R;
            } else if (diff.x < 0) {
                sprite = SNAKE_TAIL_L;
            } else if (diff.y > 0) {
                sprite = SNAKE_TAIL_T;
            } else {
                sprite = SNAKE_TAIL_B;
            }
        } else {
            Vec2i prevDiff = {
                mSnakeParts[i-1].x - mSnakeParts[i].x,
                mSnakeParts[i-1].y - mSnakeParts[i].y
            };
            Vec2i nextDiff = {
                mSnakeParts[i].x - mSnakeParts[i+1].x,
                mSnakeParts[i].y - mSnakeParts[i+1].y
            };

            if (std::abs(prevDiff.x) > 15) {
                prevDiff.x = prevDiff.x > 0 ? -1 : 1;
            }
            if (std::abs(prevDiff.y) > 12) {
                prevDiff.y = prevDiff.y > 0 ? -1 : 1;
            }

            if (std::abs(nextDiff.x) > 15) {
                nextDiff.x = nextDiff.x > 0 ? -1 : 1;
            }
            if (std::abs(nextDiff.y) > 12) {
                nextDiff.y = nextDiff.y > 0 ? -1 : 1;
            }

            if ((prevDiff.x * nextDiff.x > 0) || (prevDiff.y * nextDiff.y > 0)) {
                if (prevDiff.x != 0) {
                    sprite = SNAKE_BODY_RL;
                } else {
                    sprite = SNAKE_BODY_TB;
                }
            } else {
                if (
                    (prevDiff.x > 0 && nextDiff.y < 0) ||
                    (prevDiff.y > 0 && nextDiff.x < 0)
                ) {
                    sprite = SNAKE_BODY_BR;
                } else if (
                    (prevDiff.x < 0 && nextDiff.y < 0) ||
                    (prevDiff.y > 0 && nextDiff.x > 0)
                ) {
                    sprite = SNAKE_BODY_BL;
                } else if (
                    (prevDiff.x > 0 && nextDiff.y > 0) ||
                    (prevDiff.y < 0 && nextDiff.x < 0)
                ) {
                    sprite = SNAKE_BODY_TR;
                } else {
                    sprite = SNAKE_BODY_TL;
                }
            }
        }
        API::Draw(SPRITES[sprite], mSnakeParts[i]);
    }
}

///////////////////////////////////////////////////////////////////////////////
void Snake::moveSnake(void)
{
    if (mAccumulatedTime >= 0.2f) {
        mAccumulatedTime = 0.0f;

        if (mNewOffset.x != -mOffset.x || mNewOffset.y != -mOffset.y) {
            mOffset = mNewOffset;
        }

        Vec2i newHead = {
            mSnakeParts.front().x + mOffset.x * 2,
            mSnakeParts.front().y + mOffset.y * 2
        };

        if (newHead.x < 0) {
            newHead.x = 30;
        } else if (newHead.x >= 31) {
            newHead.x = 0;
        }

        if (newHead.y < 4) {
            newHead.y = 26;
        } else if (newHead.y >= 28) {
            newHead.y = 4;
        }

        for (size_t i = 0; i < mSnakeParts.size() - 1; i++) {
            if (newHead.x == mSnakeParts[i].x && newHead.y == mSnakeParts[i].y) {
                mGameOver = true;
                API::PushEvent(API::Event::CORE, API::Event::GameOver{mScore});
                return;
            }
        }

        mSnakeParts.push_front(newHead);

        if (newHead.x == mApplePosition.x && newHead.y == mApplePosition.y) {
            mScore += 10;
            respawnApple();
        } else {
            mSnakeParts.pop_back();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void Snake::drawScore(void)
{
    Text("SCORE", TextColor::TEXT_WHITE, Vec2i{2, 2});
    Text("BEST SCORE", TextColor::TEXT_WHITE, Vec2i{15, 2});
    Text(std::to_string(mScore), TextColor::TEXT_WHITE, Vec2i{8, 2});
    Text(std::to_string(mBestScore), TextColor::TEXT_WHITE, Vec2i{26, 2});
}

///////////////////////////////////////////////////////////////////////////////
void Snake::Tick(float deltaSeconds)
{
    mAccumulatedTime += deltaSeconds;

    while (auto event = API::PollEvent(API::Event::GAME)) {
        if (auto key = event->GetIf<API::Event::KeyPressed>()) {
            handleKeyPressed(key->code);
        } else if (auto best = event->GetIf<API::Event::BestScore>()) {
            mBestScore = best->score > mScore ? best->score : mScore;
        }
    }

    if (!mGameOver) {
        for (size_t y = 4; y < 28; y += 2) {
            for (size_t x = 0; x < 31; x += 2) {
                API::Draw(SPRITES[EMPTY], Vec2i(x, y));
            }
        }
        if (mIngame) {
            moveSnake();
            API::Draw(SPRITES[APPLE], mApplePosition);
        } else {
            Text("PRESS SPACE TO START", TextColor::TEXT_WHITE, Vec2i{5, 25});
        }
        drawSpritesSnake();
        drawScore();
    } else {
        drawSpritesSnake();
        Text("GAME OVER", TextColor::TEXT_WHITE, Vec2i{11, 8});
        Text("SCORE", TextColor::TEXT_WHITE, Vec2i{13, 10});
        Text(std::to_string(mScore), TextColor::TEXT_WHITE, Vec2i{14, 12});
        Text("PRESS SPACE TO RESTART", TextColor::TEXT_WHITE, Vec2i{5, 25});
    }
}

///////////////////////////////////////////////////////////////////////////////
bool Snake::IsGameOver(void) const
{
    return (mGameOver);
}

///////////////////////////////////////////////////////////////////////////////
int Snake::GetScore(void) const
{
    return (mScore);
}

///////////////////////////////////////////////////////////////////////////////
std::string Snake::GetName(void) const
{
    return ("Snake");
}

///////////////////////////////////////////////////////////////////////////////
std::string Snake::GetDescription(void) const
{
    return ("Snake game");
}

///////////////////////////////////////////////////////////////////////////////
std::string Snake::GetSpriteSheet(void) const
{
    return ("assets/SNAKE/sprite.png");
}

} // namespace Arc
