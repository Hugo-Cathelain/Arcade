///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "games/NIBBLER/Game.hpp"
#include "Arcade/core/API.hpp"
#include "games/NIBBLER/Assets.hpp"
#include "games/NIBBLER/Maps/Map.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>

///////////////////////////////////////////////////////////////////////////////
// Namespace Arc
///////////////////////////////////////////////////////////////////////////////
namespace Arc::Nibbler
{

///////////////////////////////////////////////////////////////////////////////
Game::Game(void)
    : mTimer(0.f)
    , mState(State::START_PRESSED)
    , mScore(0)
    , mLifes(3)
    , mLevel(0)
    , mTimerGame(90)
    , mAnimationTimer(0.f)
{
    // Initialize random seed
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // Create snake, map, and fruits
    mSnake = std::make_unique<Snake>();
    mMap = std::make_unique<Maps>();

    // Add initial fruit
    mFruits[0] = std::make_unique<Fruit>();
}

///////////////////////////////////////////////////////////////////////////////
Game::~Game()
{}

///////////////////////////////////////////////////////////////////////////////
void Game::BeginPlay(void)
{
    API::PushEvent(
        API::Event::GRAPHICS,
        API::Event::GridSize({ARCADE_SCREEN_WIDTH, ARCADE_SCREEN_HEIGHT})
    );

    // Reset the game to level 1
    ResetGame(1);
}

///////////////////////////////////////////////////////////////////////////////
void Game::EndPlay(void)
{}


///////////////////////////////////////////////////////////////////////////////
void Game::DrawSpawningAnimation(void)
{
    int progress = mAnimationTimer / 1.f * 11;

    for (int y = 0; y < 3; y++) {
        for (int i = progress; i < 11; i++) {
            API::Draw(SPRITES[TRON_SQUARE], Vec2i(7 + i, 28 + y));
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void Game::InitFruit(int level)
{
    // Get the fruit positions for the current level
    const auto& levelFruits = MAPS_FRUIT[(level - 1) % MAPS_FRUIT.size()];

    // Clear existing fruits first
    mFruits.clear();

    // Create fruits at each position until we reach the end (sentinel value or size limit)
    for (size_t i = 0; i < levelFruits.size(); ++i) {
        // Check if this is a valid fruit position (not a sentinel)
        if (levelFruits[i].x == -1 && levelFruits[i].y == -1)
            break;

        // Create new fruit and set its position
        mFruits[i] = std::make_unique<Fruit>();
        mFruits[i]->SetPosition({
            levelFruits[i].x,
            levelFruits[i].y + ARCADE_OFFSET_Y
        });
    }
}

///////////////////////////////////////////////////////////////////////////////
void Game::Tick(float deltaSeconds)
{
    mTimer += deltaSeconds;

    // Timer for decreasing game time
    static float gameTimeCounter = 0.0f;

    // Increase the animation timer
    if (mState == State::START_PRESSED) {
        mAnimationTimer += deltaSeconds;
        if (mAnimationTimer > 1.f) {
            mState = State::PLAYING;
            mAnimationTimer = 0.f;
        }
    }

    // Handle events
    HandleEvents();

    // Updating
    if (mState == State::PLAYING) {
        mSnake->Update(deltaSeconds);

        CheckForFruitsEaten();
        CheckForAllFruitsEaten();
        gameTimeCounter += deltaSeconds;

        // Check if a second has passed
        if (gameTimeCounter >= 1.0f) {
            // Decrement timer and reset counter
            if (mTimerGame > 0) {
                mTimerGame--;
            }
            gameTimeCounter -= 1.0f; // Subtract one second, preserving any remainder
        }
    }
    // Draw everything
    Draws();
}

///////////////////////////////////////////////////////////////////////////////
void Game::Text(
    const std::string& text,
    Game::TextColor color,
    const Vec2i& position
)
{
    static const int LTRS_OFFSET_Y = 13;
    static const Color LTS_COLORS[] = {
        Color{255, 255, 222}, Color{0, 184, 151}, Color{255, 0, 0},
        Color{255, 255, 0}, Color{204, 29, 243}, Color{5, 12, 196},
        Color{168, 168, 139}
    };

    for (size_t i = 0; i < text.size(); i++) {
        int row = 0;
        int col = 0;

        if (text[i] >= 'A' && text[i] <= 'Z') {
            col = static_cast<int>(text[i] - 'A');
        } else if (text[i] >= '0' && text[i] <= '9') {
            col = static_cast<int>(text[i] - '0');
            row = 1;
        } else if (text[i] == ',') {
            col = 10;
            row = 1;
        } else if (text[i] == '@') {
            col = 26;
            row = 0;
        } else {
            continue;
        }

        IGameModule::Asset sprite(
            {col, LTRS_OFFSET_Y + row + static_cast<int>(color) * 2},
            std::string(1, text[i]),
            LTS_COLORS[static_cast<int>(color)]
        );

        API::Draw(sprite, position + Vec2i{static_cast<int>(i), 0});
    }
}

///////////////////////////////////////////////////////////////////////////////
void Game::DrawScore(void)
{
    API::Draw(SPRITES[TEXT_PLAYER], Vec2i(1, 0));
    API::Draw(SPRITES[TEXT_1], Vec2i(3, 0));
    int place = 0;
    if (mScore > 9999) {
        place = 2;
    } else if (mScore > 99) {
        place = 1;
    }
    Text(std::to_string(mScore), TextColor::TEXT_WHITE, {15 - place, 0});

    API::Draw(SPRITES[TEXT_LEFT], Vec2i(22, 0));
    Text(std::to_string(mSnake->GetLives()), TextColor::TEXT_WHITE, {26, 0});

    API::Draw(SPRITES[TEXT_HISCORE], Vec2i(2, 3));
    Text("50,000", TextColor::TEXT_CYAN, {10, 3});

    Text("TIME", TextColor::TEXT_YELLOW, {19, 3});
    Text(std::to_string(mTimerGame), TextColor::TEXT_WHITE, {26, 3});

    Text("C", TextColor::TEXT_WHITE, {27, 12});
    Text("R", TextColor::TEXT_WHITE, {27, 13});
    Text("E", TextColor::TEXT_WHITE, {27, 14});
    Text("D", TextColor::TEXT_WHITE, {27, 15});
    Text("I", TextColor::TEXT_WHITE, {27, 16});
    Text("T", TextColor::TEXT_WHITE, {27, 17});
    Text("0", TextColor::TEXT_WHITE, {27, 19});

    Text("WAVE", TextColor::TEXT_WHITE, {10, 31});
    Text(std::to_string(mLevel - 1), TextColor::TEXT_WHITE, {16, 31});
}

///////////////////////////////////////////////////////////////////////////////
void Game::Draws(void)
{
    // Draw map
    if (mMap) {
        mMap->DrawMap(mLevel - 1);
    }

    // Draw fruits
    for (auto& [id, fruit] : mFruits) {
        if (fruit) {
            fruit->DrawAnimation(mTimer);
        }
    }

    // Draw snake
    if (mSnake) {
        mSnake->Draw(mTimer);
    }

    // Draw score
    DrawScore();

    // Draw lives
    DrawSnakeLives();

    if (mState != State::PLAYING) {
        DrawSpawningAnimation();
    }
}

///////////////////////////////////////////////////////////////////////////////
void Game::HandleEvents(void)
{
    Vec2i desiredDirection(0);

    // Process input events
    while (auto event = API::PollEvent(API::Event::GAME)) {
        if (auto key = event->GetIf<API::Event::KeyPressed>()) {
            // Handle key presses based on game state
            switch (mState) {
                case State::PRESS_START:
                    if (key->code == EKeyboardKey::SPACE) {
                        mState = State::START_PRESSED;
                    }
                    break;
                case State::START_PRESSED:
                    mAnimationTimer = 0.f;
                    break;
                case State::PLAYING:
                    if (key->code == EKeyboardKey::UP) {
                        desiredDirection = Vec2i{0, -1};
                    } else if (key->code == EKeyboardKey::DOWN) {
                        desiredDirection = Vec2i{0, 1};
                    } else if (key->code == EKeyboardKey::LEFT) {
                        desiredDirection = Vec2i{-1, 0};
                    } else if (key->code == EKeyboardKey::RIGHT) {
                        desiredDirection = Vec2i{1, 0};
                    }
                    if (desiredDirection != 0) {
                        mSnake->SetDesiredDirection(desiredDirection);
                    }
                    break;
                case State::DEATH_ANIMATION:
                    // No input during death animation
                    break;
                default:
                    break;
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void Game::CheckForFruitsEaten(void)
{
    if (mState != State::PLAYING || !mSnake)
        return;

    Vec2i snakePos = mSnake->GetPosition();
    for (auto it = mFruits.begin(); it != mFruits.end();) {
        if (it->second->GetPosition() == snakePos) {
            // Fruit eaten, remove it from the game
            mScore += 10; // Example score increment
            it = mFruits.erase(it);
            mSnake->Grow(); // Example snake growth
        } else {
            ++it;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void Game::CheckForAllFruitsEaten(void)
{
    if (mState != State::PLAYING || !mFruits.empty())
        return;

    if (mFruits.size() > 0) {
        return; // Not all fruits eaten
    }
    mState = State::START_PRESSED;
    // All fruits eaten, go to next level
    mLevel++;
    ResetGame(mLevel);
}

///////////////////////////////////////////////////////////////////////////////
void Game::ResetGame(int level)
{
    // Initialize level
    mLevel = level;
    mTimerGame = 90;

    // Reset snake
    mSnake->Reset();

    // Clear fruits and add new ones based on level
    mFruits.clear();
    InitFruit(level);

    // Reset map and snake for the level
    mMap->SetLevel(level);
    mSnake->SetLevel(level - 1);

    // Reset game state
    mState = State::START_PRESSED;
}

///////////////////////////////////////////////////////////////////////////////
void Game::DrawSnakeLives(void)
{
    // Draw lives indicator
    // Implementation to show remaining lives
}

} // namespace Arc
