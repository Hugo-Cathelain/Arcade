///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "games/NIBBLER/Nibbler.hpp"
#include "Arcade/core/API.hpp"
#include "games/NIBBLER/Assets.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>

///////////////////////////////////////////////////////////////////////////////
// Namespace Arc
///////////////////////////////////////////////////////////////////////////////
namespace Arc::Nibbler
{

///////////////////////////////////////////////////////////////////////////////
Nibbler::Nibbler(void)
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
Nibbler::~Nibbler()
{}

///////////////////////////////////////////////////////////////////////////////
void Nibbler::BeginPlay(void)
{
    API::PushEvent(API::Event::GRAPHICS, API::Event::GridSize({31, 28}));

    // Reset the game to level 1
    ResetGame(1);
}

///////////////////////////////////////////////////////////////////////////////
void Nibbler::EndPlay(void)
{}

///////////////////////////////////////////////////////////////////////////////
void Nibbler::Tick(float deltaSeconds)
{
    mTimer += deltaSeconds;

    // Handle events
    HandleEvents();

    // Check for collisions with fruits
    CheckForFruitsEaten();

    // Check if all fruits are eaten
    CheckForAllFruitsEaten();

    // Handle snake movement speed
    HandleSnakeSpeed();

    // Draw everything
    Draws();
}

///////////////////////////////////////////////////////////////////////////////
void Nibbler::DrawScore(void)
{
    // Implementation of DrawScore
    // Display score on screen
}

///////////////////////////////////////////////////////////////////////////////
void Nibbler::Draws(void)
{
    // Draw map
    if (mMap)
        mMap->DrawMap(mLevel);

    // Draw fruits
    for (auto& [id, fruit] : mFruits) {
        if (fruit)
            fruit->DrawAnimation(mTimer);
    }

    // Draw snake
    if (mSnake)
        mSnake->Draw(mTimer);

    // Draw score
    DrawScore();

    // Draw lives
    DrawSnakeLives();
}

///////////////////////////////////////////////////////////////////////////////
void Nibbler::HandleEvents(void)
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
void Nibbler::CheckForFruitsEaten(void)
{
    if (mState != State::PLAYING || !mSnake)
        return;

    // // Check if snake head collides with any fruit
    // for (auto it = mFruits.begin(); it != mFruits.end(); ) {
    //     if (it->second && mSnake->CheckCollisionWithFruit(*it->second)) {
    //         // Snake ate fruit, increase score and snake length
    //         mScore += it->second->GetValue();
    //         mSnake->Grow();

    //         // Remove eaten fruit
    //         it = mFruits.erase(it);
    //     } else {
    //         ++it;
    //     }
    // }
}

///////////////////////////////////////////////////////////////////////////////
void Nibbler::CheckForAllFruitsEaten(void)
{
    if (mState != State::PLAYING || !mFruits.empty())
        return;

    // All fruits eaten, go to next level
    mLevel++;
    ResetGame(mLevel);
}

///////////////////////////////////////////////////////////////////////////////
void Nibbler::ResetGame(int level)
{
    // Initialize level
    mLevel = level;

    // Reset snake
    // mSnake->Reset();

    // Clear fruits and add new ones based on level
    mFruits.clear();
    for (int i = 0; i < level; i++) {
        mFruits[i] = std::make_unique<Fruit>();
    }

    // Reset map for the level
    mMap->SetLevel(level);

    // Reset game state
    mState = State::PLAYING;
}

///////////////////////////////////////////////////////////////////////////////
void Nibbler::DrawSnakeLives(void)
{
    // Draw lives indicator
    // Implementation to show remaining lives
}

///////////////////////////////////////////////////////////////////////////////
void Nibbler::HandleSnakeSpeed(void)
{
    if (mState == State::PLAYING && mSnake) {
        // Update snake movement based on timer
        mSnake->Update(mTimer);
    }
}

///////////////////////////////////////////////////////////////////////////////
bool Nibbler::IsGameOver(void) const
{
    return (false);
}

///////////////////////////////////////////////////////////////////////////////
int Nibbler::GetScore(void) const
{
    return (0);
}

///////////////////////////////////////////////////////////////////////////////
std::string Nibbler::GetName(void) const
{
    return ("Arcade: Nibbler (1982)");
}

///////////////////////////////////////////////////////////////////////////////
std::string Nibbler::GetDescription(void) const
{
    return ("Nibbler game");
}

///////////////////////////////////////////////////////////////////////////////
std::string Nibbler::GetSpriteSheet(void) const
{
    return ("assets/NIBBLER/sprites.png");
}

} // namespace Arc
