///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "games/PACMAN/Game.hpp"
#include "games/PACMAN/Assets.hpp"
#include "games/PACMAN/Menu.hpp"
#include "../../Arcade/interfaces/IGameModule.hpp"
#include "../../Arcade/core/API.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Arc::Pacman
///////////////////////////////////////////////////////////////////////////////
namespace Arc::Pacman
{

///////////////////////////////////////////////////////////////////////////////
Game::Game(void)
    : mTimer(0.f)
    , mState(State::PRESS_START)
    , mScore(0)
    , mEatenGhost(Ghost::Type::NONE)
    , mPowerPillTimer(0.f)
    , mKillCount(0)
    , mHealth(4)
    , mLevel(1)
{}

///////////////////////////////////////////////////////////////////////////////
Game::~Game()
{}

///////////////////////////////////////////////////////////////////////////////
void Game::SetDefaultGums(void)
{
    mGums.clear();

    // Small gums
    for (int y = 0; y < ARCADE_GAME_HEIGHT; y++) {
        for (int x = 0; x < ARCADE_GAME_WIDTH; x++) {
            if (y >= 9 && y <= 19 && x != 6 && x != 21) {
                continue;
            }

            if ((x == 13 || x == 14) && y == 23) {
                continue;
            }

            if (PACMAN_MAP[y][x] == TILE_EMPTY) {
                mGums[y * ARCADE_GAME_WIDTH + x] = GumType::SMALL;
            }
        }
    }

    // Big gums
    mGums[3 * ARCADE_GAME_WIDTH + 1] = GumType::BIG;
    mGums[3 * ARCADE_GAME_WIDTH + 26] = GumType::BIG;
    mGums[23 * ARCADE_GAME_WIDTH + 1] = GumType::BIG;
    mGums[23 * ARCADE_GAME_WIDTH + 26] = GumType::BIG;
}

///////////////////////////////////////////////////////////////////////////////
void Game::DrawMapBaseLayer(void)
{
    for (int y = 0; y < ARCADE_GAME_HEIGHT; y++) {
        for (int x = 0; x < ARCADE_GAME_WIDTH; x++) {
            API::Draw(SPRITES[PACMAN_MAP[y][x]], Vec2i{x, y + ARCADE_OFFSET_Y});
        }
    }

    if (mState != State::PLAYING) {
        if (mState == State::PRESS_START) {
            Menu::Text(
                "PLAYER ONE", Menu::TextColor::TEXT_CYAN,
                Vec2i{9, 11 + ARCADE_OFFSET_Y}
            );
        }

        Menu::Text(
            "READY!", Menu::TextColor::TEXT_YELLOW,
            Vec2i{11, 17 + ARCADE_OFFSET_Y}
        );
    }
}

///////////////////////////////////////////////////////////////////////////////
void Game::DrawScore(void)
{
    Menu::Text("1UP   HIGH SCORE",
        Menu::TextColor::TEXT_WHITE, Vec2i{3, 0});

    std::string score = std::to_string(mScore);
    if (mScore < 10) {
        score = "0" + score;
    }

    Menu::Text(
        score,
        Menu::TextColor::TEXT_WHITE,
        Vec2i{7 - static_cast<int>(score.size()), 1}
    );
}

///////////////////////////////////////////////////////////////////////////////
void Game::DrawGums(void)
{
    for (const auto& [index, type] : mGums) {
        Vec2i position(
            index % ARCADE_GAME_WIDTH,
            index / ARCADE_GAME_WIDTH + ARCADE_OFFSET_Y
        );

        if (type == GumType::SMALL) {
            API::Draw(SPRITES[TILE_POINT], position);
        } else {
            API::Draw(SPRITES[TILE_PACGUM], position);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void Game::DrawEatScore(void)
{
    static const SpriteType SCORES[] = {
        SCORE_200, SCORE_400, SCORE_800, SCORE_1600
    };

    for (const auto& [score, timer] : mEatTimer) {
        if (std::get<0>(timer) >= 2.f) {
            mEatTimer.erase(score);
            continue;
        }

        API::Draw(
            SPRITES[SCORES[score / 200 - 1]],
            std::get<1>(timer) + Vec2i(0, ARCADE_OFFSET_Y)
        );
    }

    if (mEatTimer.size() == 0) {
        mEatenGhost = Ghost::Type::NONE;
    }
}

///////////////////////////////////////////////////////////////////////////////
void Game::HandleEvents(void)
{
    while (auto event = API::PollEvent(API::Event::GAME)) {
        if (auto key = event->GetIf<API::Event::KeyPressed>()) {
            Vec2i direction(0);

            if (key->code == EKeyboardKey::UP) {
                direction = Vec2i{0, -1};
            } else if (key->code == EKeyboardKey::DOWN) {
                direction = Vec2i{0, 1};
            } else if (key->code == EKeyboardKey::LEFT) {
                direction = Vec2i{-1, 0};
            } else if (key->code == EKeyboardKey::RIGHT) {
                direction = Vec2i{1, 0};
            }

            if (key->code == EKeyboardKey::SPACE && mState == State::PRESS_START) {
                mState = State::START_PRESSED;
            }

            if (direction != 0 && mState == State::START_PRESSED) {
                mState = State::PLAYING;
                mPlayer->SetPosition({14 + direction.x, 23});
            }

            if (direction != 0 && mState == State::PLAYING) {
                mPlayer->SetDesiredDirection(direction);
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void Game::HandlePowerPill(float deltaSeconds)
{
    if (mPowerPillTimer == 0.f) {
        return;
    }

    mPowerPillTimer -= deltaSeconds;
    if (mPowerPillTimer < 0.f) {
        mPowerPillTimer = 0.f;
        mKillCount = 0;
        if (mBlinky->GetState() == Ghost::State::FRIGHTENED) {
            mBlinky->SetState(Ghost::State::CHASE);
        }
        if (mPinky->GetState() == Ghost::State::FRIGHTENED) {
            mPinky->SetState(Ghost::State::CHASE);
        }
        if (mInky->GetState() == Ghost::State::FRIGHTENED) {
            mInky->SetState(Ghost::State::CHASE);
        }
        if (mClyde->GetState() == Ghost::State::FRIGHTENED) {
            mClyde->SetState(Ghost::State::CHASE);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void Game::CheckForGumsEaten(void)
{
    for (auto& [index, type] : mGums) {
        Vec2i position(index % ARCADE_GAME_WIDTH, index / ARCADE_GAME_WIDTH);

        if (mPlayer->GetPosition() == position) {
            if (type == GumType::SMALL) {
                mScore += 10;
            } else {
                mPowerPillTimer = 10.f;
                mScore += 50;
                if (mBlinky->GetState() == Ghost::State::CHASE) {
                    mBlinky->SetState(Ghost::State::FRIGHTENED);
                }
                if (mPinky->GetState() == Ghost::State::CHASE) {
                    mPinky->SetState(Ghost::State::FRIGHTENED);
                }
                if (mInky->GetState() == Ghost::State::CHASE) {
                    mInky->SetState(Ghost::State::FRIGHTENED);
                }
                if (mClyde->GetState() == Ghost::State::CHASE) {
                    mClyde->SetState(Ghost::State::FRIGHTENED);
                }
            }

            mGums.erase(index);
            break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void Game::BeginPlay(void)
{
    mTimer = 0.0f;
    mState = State::PRESS_START;
    mPlayer.reset(new Player());
    mBlinky.reset(new Ghost(Ghost::Type::BLINKY));
    mPinky.reset(new Ghost(Ghost::Type::PINKY));
    mInky.reset(new Ghost(Ghost::Type::INKY));
    mClyde.reset(new Ghost(Ghost::Type::CLYDE));
    mEatenGhost = Ghost::Type::NONE;
    mScore = 0;
    mPowerPillTimer = 0.f;
    mKillCount = 0;
    mEatTimer.clear();
    mHealth = 4;
    mLevel = 1;
    SetDefaultGums();
}

///////////////////////////////////////////////////////////////////////////////
void Game::CheckForGhostsCollisions(std::unique_ptr<Ghost>& ghost)
{
    if (ghost->GetPosition() != mPlayer->GetPosition()) {
        return;
    }

    if (ghost->GetState() == Ghost::State::FRIGHTENED) {
        mEatenGhost = ghost->GetType();
        ghost->SetState(Ghost::State::EATEN);
        mScore += 200 << mKillCount;
        mEatTimer[200 << mKillCount] =
            std::make_tuple(0.f, mPlayer->GetPosition());
        mKillCount++;
    } else if (ghost->GetState() != Ghost::State::EATEN) {
        mHealth--;
        mTimer = 0.f;
        mState = State::START_PRESSED;
        if (mHealth == 0) {
            API::PushEvent(API::Event::GAME, API::Event::GameOver{mScore});
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void Game::Tick(float deltaSeconds)
{
    if (mState != State::PRESS_START) {
        mTimer += deltaSeconds;
    }

    for (auto& [score, timer] : mEatTimer) {
        std::get<0>(timer) += deltaSeconds;
    }

    // Handle Events
    HandleEvents();

    // Updating
    if (mState == State::PLAYING) {
        HandlePowerPill(deltaSeconds);
        if (mEatTimer.size() == 0) {
            mPlayer->Update(deltaSeconds);

            Vec2i blinkyPos = mBlinky->GetPosition();
            mBlinky->Update(deltaSeconds, mPlayer, blinkyPos);
            mPinky->Update(deltaSeconds, mPlayer, blinkyPos);
            mInky->Update(deltaSeconds, mPlayer, blinkyPos);
            mClyde->Update(deltaSeconds, mPlayer, blinkyPos);
        }
        CheckForGhostsCollisions(mBlinky);
        CheckForGhostsCollisions(mPinky);
        CheckForGhostsCollisions(mInky);
        CheckForGhostsCollisions(mClyde);
        CheckForGumsEaten();
    }

    // Drawing
    DrawMapBaseLayer();
    DrawGums();
    DrawEatScore();

    if (mState != State::PRESS_START) {
        if (mState != State::DEATH_ANIMATION) {
            if (mEatTimer.size() == 0 || mBlinky->GetType() != mEatenGhost) {
                mBlinky->Draw(mTimer);
            }
            if (mEatTimer.size() == 0 || mPinky->GetType() != mEatenGhost) {
                mPinky->Draw(mTimer);
            }
            if (mEatTimer.size() == 0 || mInky->GetType() != mEatenGhost) {
                mInky->Draw(mTimer);
            }
            if (mEatTimer.size() == 0 || mClyde->GetType() != mEatenGhost) {
                mClyde->Draw(mTimer);
            }
        }
        if (mEatTimer.size() == 0) {
            mPlayer->Draw(mTimer);
        }
    }

    DrawScore();
}

///////////////////////////////////////////////////////////////////////////////
void Game::EndPlay(void)
{
    mGums.clear();
    mPlayer.reset();
    mBlinky.reset();
    mPinky.reset();
    mInky.reset();
    mClyde.reset();
    mEatTimer.clear();
}

} // namespace Arc::Pacman
