///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "games/PACMAN/Game.hpp"
#include "games/PACMAN/Assets.hpp"
#include "games/PACMAN/Menu.hpp"
#include "../../Arcade/interfaces/IGameModule.hpp"
#include "../../Arcade/core/API.hpp"
#include "games/PACMAN/Random.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Arc::Pacman
///////////////////////////////////////////////////////////////////////////////
namespace Arc::Pacman
{

///////////////////////////////////////////////////////////////////////////////
const std::vector<float> Game::SPEEDS[4] = {
    {.80, .71, .90, .79, .75, .50, .40},
    {.90, .79, .95, .83, .85, .55, .45},
    {1.0, .87, 1.0, .87, .95, .60, .50},
    {.90, .79, .90, .90, .95, .95, .50}
};

///////////////////////////////////////////////////////////////////////////////
const std::vector<float> Game::MODES[3] = {
    {7, 20, 7, 20, 5, 20, 5, std::numeric_limits<float>::infinity()},
    {7, 20, 7, 20, 5, 1033, 1/60, std::numeric_limits<float>::infinity()},
    {5, 20, 5, 20, 5, 1037, 1/60, std::numeric_limits<float>::infinity()}
};

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
    , mModeTimer(0.f)
    , mModeIndex(0)
    , mSoundTimer(0.f)
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

    if (mState != State::PLAYING && mState != State::DEATH_ANIMATION) {
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
            break;
        }

        API::Draw(
            SPRITES[SCORES[static_cast<int>(std::log2(score / 200))]],
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
            mBlinky->SetMovementPercentage(
                mSpeeds[static_cast<int>(SpeedType::GHOST_NORM)]
            );
        }
        if (mPinky->GetState() == Ghost::State::FRIGHTENED) {
            mPinky->SetState(Ghost::State::CHASE);
            mPinky->SetMovementPercentage(
                mSpeeds[static_cast<int>(SpeedType::GHOST_NORM)]
            );
        }
        if (mInky->GetState() == Ghost::State::FRIGHTENED) {
            mInky->SetState(Ghost::State::CHASE);
            mInky->SetMovementPercentage(
                mSpeeds[static_cast<int>(SpeedType::GHOST_NORM)]
            );
        }
        if (mClyde->GetState() == Ghost::State::FRIGHTENED) {
            mClyde->SetState(Ghost::State::CHASE);
            mClyde->SetMovementPercentage(
                mSpeeds[static_cast<int>(SpeedType::GHOST_NORM)]
            );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void Game::CheckForGumsEaten(void)
{
    for (auto& [index, type] : mGums) {
        Vec2i position(index % ARCADE_GAME_WIDTH, index / ARCADE_GAME_WIDTH);

        if (mPlayer->GetPosition() == position) {
            API::PlaySound(mGums.size() % 2 ? SFX_EAT_DOT_0 : SFX_EAT_DOT_1);

            if (type == GumType::SMALL) {
                mBlinky->IncrementCounter();
                mPinky->IncrementCounter();
                mInky->IncrementCounter();
                mClyde->IncrementCounter();
                mScore += 10;
            } else {
                mPowerPillTimer = 10.f;
                mScore += 50;
                if (mBlinky->GetState() != Ghost::State::EATEN) {
                    mBlinky->SetState(Ghost::State::FRIGHTENED);
                    mBlinky->SetMovementPercentage(
                        mSpeeds[static_cast<int>(SpeedType::GHOST_FRIGHT)]
                    );
                }
                if (mPinky->GetState() != Ghost::State::EATEN) {
                    mPinky->SetState(Ghost::State::FRIGHTENED);
                    mPinky->SetMovementPercentage(
                        mSpeeds[static_cast<int>(SpeedType::GHOST_FRIGHT)]
                    );
                }
                if (mInky->GetState() != Ghost::State::EATEN) {
                    mInky->SetState(Ghost::State::FRIGHTENED);
                    mInky->SetMovementPercentage(
                        mSpeeds[static_cast<int>(SpeedType::GHOST_FRIGHT)]
                    );
                }
                if (mClyde->GetState() != Ghost::State::EATEN) {
                    mClyde->SetState(Ghost::State::FRIGHTENED);
                    mClyde->SetMovementPercentage(
                        mSpeeds[static_cast<int>(SpeedType::GHOST_FRIGHT)]
                    );
                }
            }

            mGums.erase(index);
            break;
        }
    }

    CheckForAllGumsEaten();
}

///////////////////////////////////////////////////////////////////////////////
void Game::BeginPlay(void)
{
    mTimer = 0.0f;
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
    ResetGame(mLevel);
    mState = State::PRESS_START;
    API::PlaySound(SFX_START);
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
        mKillCount = mKillCount % 4;
        ghost->SetMovementPercentage(1.f);
        API::PlaySound(SFX_EAT_GHOST);
    } else if (ghost->GetState() != Ghost::State::EATEN) {
        API::PlaySound(SFX_DEATH_0);
        mHealth--;
        mTimer = 0.f;
        mState = State::DEATH_ANIMATION;
        if (mHealth == 0) {
            API::PushEvent(API::Event::GAME, API::Event::GameOver{mScore});
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void Game::CheckForAllGumsEaten(void)
{
    if (mGums.size() == 0) {
        ResetGame(mLevel + 1);
    }
}

///////////////////////////////////////////////////////////////////////////////
void Game::ResetGame(int level)
{
    mLevel = level;

    if (level == 1) {
        mSpeeds = SPEEDS[0];
        mModeTimers = MODES[0];
    } else if (level >= 2 && level <= 4) {
        mSpeeds = SPEEDS[1];
        mModeTimers = MODES[1];
    } else if (level >= 5 && level <= 20) {
        mSpeeds = SPEEDS[2];
        mModeTimers = MODES[2];
    } else {
        mSpeeds = SPEEDS[3];
        mModeTimers = MODES[2];
    }

    SetDefaultGums();
    mTimer = 0.0f;
    mState = State::START_PRESSED;
    mPlayer.reset(new Player());
    mBlinky.reset(new Ghost(Ghost::Type::BLINKY));
    mPinky.reset(new Ghost(Ghost::Type::PINKY));
    mInky.reset(new Ghost(Ghost::Type::INKY));
    mClyde.reset(new Ghost(Ghost::Type::CLYDE));
    mEatenGhost = Ghost::Type::NONE;
    mPowerPillTimer = 0.f;
    mKillCount = 0;
    mEatTimer.clear();
}

///////////////////////////////////////////////////////////////////////////////
void Game::DrawPacmanLives(void)
{
    auto liveSprite = IGameModule::Asset({2, 2}, "()", CLR_YELLOW, {16, 16});

    for (int i = 0; i < mHealth; i++) {
        API::Draw(liveSprite, Vec2f(
            2.5f + 2.f * i,
            ARCADE_SCREEN_HEIGHT - 1.5f
        ));
    }
}

///////////////////////////////////////////////////////////////////////////////
void Game::HandlePacmanSpeed(void)
{
    Vec2i direction = mPlayer->GetDirection();
    Vec2i position = mPlayer->GetPosition();
    Vec2i nextTile = position + direction;
    int index = nextTile.y * ARCADE_GAME_WIDTH + nextTile.x;

    if (mGums.count(index)) {
        if (mPowerPillTimer > 0.f) {
            mPlayer->SetMovementPercentage(
                mSpeeds[static_cast<int>(SpeedType::PACMAN_FIGHT_DOTS)]
            );
        } else {
            mPlayer->SetMovementPercentage(
                mSpeeds[static_cast<int>(SpeedType::PACMAN_NORM_DOTS)]
            );
        }
    } else {
        if (mPowerPillTimer > 0.f) {
            mPlayer->SetMovementPercentage(
                mSpeeds[static_cast<int>(SpeedType::PACMAN_FRIGHT)]
            );
        } else {
            mPlayer->SetMovementPercentage(
                mSpeeds[static_cast<int>(SpeedType::PACMAN_NORM)]
            );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void Game::HandleAmbiantSound(void)
{
    bool isOneEaten = false;

    isOneEaten = mBlinky->GetState() == Ghost::State::EATEN ? true : isOneEaten;
    isOneEaten = mPinky->GetState() == Ghost::State::EATEN ? true : isOneEaten;
    isOneEaten = mInky->GetState() == Ghost::State::EATEN ? true : isOneEaten;
    isOneEaten = mClyde->GetState() == Ghost::State::EATEN ? true : isOneEaten;

    if (isOneEaten) {
        if (mSoundTimer > 0.26f) {
            API::PlaySound(SFX_EYES);
            mSoundTimer = 0.f;
        }
    } else if (mPowerPillTimer > 0.f) {
        if (mSoundTimer > 0.15f) {
            API::PlaySound(SFX_FRIGHT_FIRSTLOOP);
            mSoundTimer = 0.f;
        }
    } else {
        if (mSoundTimer > 0.40f) {
            API::PlaySound(SFX_SIREN1);
            mSoundTimer = 0.f;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void Game::Tick(float deltaSeconds)
{
    mTimer += deltaSeconds;
    mSoundTimer += deltaSeconds;

    if (mState == State::PRESS_START) {
        if (mTimer >= 3.f) {
            mState = State::START_PRESSED;
            mTimer = 0.f;
        }
    }

    for (auto& [score, timer] : mEatTimer) {
        std::get<0>(timer) += deltaSeconds;
    }

    if (mState == State::PLAYING) {
        mModeTimer += deltaSeconds;
        if (mModeTimer >= mModeTimers[mModeIndex]) {
            mModeTimer = 0.f;
            mModeIndex = (mModeIndex + 1) % mModeTimers.size();
        }

        Ghost::State state = mModeIndex % 2 == 0
            ? Ghost::State::CHASE
            : Ghost::State::SCATTER;

        if (mBlinky->GetState() != Ghost::State::EATEN &&
            mBlinky->GetState() != Ghost::State::FRIGHTENED
        ) {
            mBlinky->SetState(state);
            mBlinky->SetMovementPercentage(
                mSpeeds[static_cast<int>(SpeedType::GHOST_NORM)]
            );
        }

        if (mPinky->GetState() != Ghost::State::EATEN &&
            mPinky->GetState() != Ghost::State::FRIGHTENED
        ) {
            mPinky->SetState(state);
            mPinky->SetMovementPercentage(
                mSpeeds[static_cast<int>(SpeedType::GHOST_NORM)]
            );
        }

        if (mInky->GetState() != Ghost::State::EATEN &&
            mInky->GetState() != Ghost::State::FRIGHTENED
        ) {
            mInky->SetState(state);
            mInky->SetMovementPercentage(
                mSpeeds[static_cast<int>(SpeedType::GHOST_NORM)]
            );
        }

        if (mClyde->GetState() != Ghost::State::EATEN &&
            mClyde->GetState() != Ghost::State::FRIGHTENED
        ) {
            mClyde->SetState(state);
            mClyde->SetMovementPercentage(
                mSpeeds[static_cast<int>(SpeedType::GHOST_NORM)]
            );
        }
    }

    // Handle Events
    HandleEvents();

    // Updating
    if (mState == State::PLAYING) {
        HandleAmbiantSound();
        HandlePowerPill(deltaSeconds);
        if (mEatTimer.size() == 0) {
            HandlePacmanSpeed();
            mPlayer->Update(deltaSeconds);

            Vec2i blinkyPos = mBlinky->GetPosition();
            mBlinky->Update(deltaSeconds, mPlayer, blinkyPos);
            mPinky->Update(deltaSeconds, mPlayer, blinkyPos);
            mInky->Update(deltaSeconds, mPlayer, blinkyPos);
            mClyde->Update(deltaSeconds, mPlayer, blinkyPos);
        }
        CheckForGumsEaten();
        CheckForGhostsCollisions(mBlinky);
        CheckForGhostsCollisions(mPinky);
        CheckForGhostsCollisions(mInky);
        CheckForGhostsCollisions(mClyde);
    }

    // Drawing
    DrawMapBaseLayer();
    DrawPacmanLives();
    DrawGums();
    DrawEatScore();

    if (mState == State::DEATH_ANIMATION) {
        mPlayer->DrawDeathAnimation(mTimer);
        if (mTimer > 2.5f) {
            ResetGame(mLevel);
        }
    }

    if (mState != State::PRESS_START && mState != State::DEATH_ANIMATION) {
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
