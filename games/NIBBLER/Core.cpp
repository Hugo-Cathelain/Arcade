///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "games/NIBBLER/Core.hpp"
#include "games/NIBBLER/Assets.hpp"
#include "games/NIBBLER/Game.hpp"
#include "games/NIBBLER/Menu.hpp"
#include "games/NIBBLER/Maps.hpp"
#include "games/NIBBLER/Snake.hpp"
#include "games/NIBBLER/Fruit.hpp"
#include "Arcade/core/API.hpp"
#include <memory>

///////////////////////////////////////////////////////////////////////////////
// Namespace Arc::Nibbler
///////////////////////////////////////////////////////////////////////////////
namespace Arc::Nibbler
{

///////////////////////////////////////////////////////////////////////////////
Core::Core(void)
    : mGameState(nullptr)
    , mInGame(false)
{
    mGameState.reset(new Menu());
    mGameState->BeginPlay();
}

///////////////////////////////////////////////////////////////////////////////
Core::~Core()
{
    mGameState.reset();
}

///////////////////////////////////////////////////////////////////////////////
void Core::BeginPlay(void)
{
    API::PushEvent(API::Event::GRAPHICS, API::Event::GridSize({
        ARCADE_SCREEN_WIDTH, ARCADE_SCREEN_HEIGHT
    }));
}

///////////////////////////////////////////////////////////////////////////////
void Core::EndPlay(void)
{
    mGameState->EndPlay();
    mGameState.reset();
}

///////////////////////////////////////////////////////////////////////////////
void Core::Tick(float deltaSeconds)
{
    if (!mInGame) {
        while (auto event = API::PollEvent(API::Event::GAME)) {
            if (auto key = event->GetIf<API::Event::KeyPressed>()) {
                if (key->code == EKeyboardKey::SPACE) {
                    mInGame = true;
                    mGameState->EndPlay();
                    mGameState.reset(new Game());
                    mGameState->BeginPlay();
                }
            } else if (auto gameOver = event->GetIf<API::Event::GameOver>()) {
                (void)gameOver;
                mInGame = false;
                mGameState->EndPlay();
                mGameState.reset(new Menu());
                mGameState->BeginPlay();
            }
        }
    }

    mGameState->Tick(deltaSeconds);
}

///////////////////////////////////////////////////////////////////////////////
bool Core::IsGameOver(void) const
{
    return (false);
}

///////////////////////////////////////////////////////////////////////////////
int Core::GetScore(void) const
{
    return (0);
}

///////////////////////////////////////////////////////////////////////////////
std::string Core::GetName(void) const
{
    return ("Arcade: Nibbler (1982)");
}

///////////////////////////////////////////////////////////////////////////////
std::string Core::GetDescription(void) const
{
    return ("Nibbler game");
}

///////////////////////////////////////////////////////////////////////////////
std::string Core::GetSpriteSheet(void) const
{
    return ("assets/NIBBLER/sprites.png");
}
}; // namespace Arc::Nibbler