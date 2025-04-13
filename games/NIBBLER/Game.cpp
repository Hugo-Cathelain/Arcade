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
    , mState(State::PRESS_START)
    , mScore(0)
    , mLifes(3)
    , mLevel(0)
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
    API::PushEvent(API::Event::GRAPHICS, API::Event::GridSize({ARCADE_SCREEN_WIDTH, ARCADE_SCREEN_HEIGHT}));

    // Reset the game to level 1
    ResetGame(1);
}

///////////////////////////////////////////////////////////////////////////////
void Game::EndPlay(void)
{}

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

    // Handle events
    HandleEvents();

    // Updating
    if (mState == State::PLAYING) {
        mSnake->Update(deltaSeconds);

        CheckForFruitsEaten();
        CheckForAllFruitsEaten();
    }
    // Draw everything
    Draws();
}

///////////////////////////////////////////////////////////////////////////////
void Game::DrawScore(void)
{
    // Implementation of DrawScore
    // Display score on screen
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
                    // Start the game when animation is done
                    mState = State::PLAYING;
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
    mState = State::PRESS_START;
    // All fruits eaten, go to next level
    mLevel++;
    ResetGame(mLevel);
}

///////////////////////////////////////////////////////////////////////////////
void Game::ResetGame(int level)
{
    // Initialize level
    mLevel = level;

    // Reset snake
    mSnake->Reset();

    // Clear fruits and add new ones based on level
    mFruits.clear();
    InitFruit(level);

    // Reset map for the level
    mMap->SetLevel(level);

    // Reset game state
    mState = State::PRESS_START;
}

///////////////////////////////////////////////////////////////////////////////
void Game::DrawSnakeLives(void)
{
    // Draw lives indicator
    // Implementation to show remaining lives
}

} // namespace Arc
