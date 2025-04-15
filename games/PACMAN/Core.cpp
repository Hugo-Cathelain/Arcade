///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "games/PACMAN/Core.hpp"
#include "../../Arcade/core/API.hpp"
#include "games/PACMAN/Assets.hpp"
#include "games/PACMAN/Menu.hpp"
#include "games/PACMAN/Game.hpp"
#include <memory>

///////////////////////////////////////////////////////////////////////////////
// Namespace Arc::Pacman
///////////////////////////////////////////////////////////////////////////////
namespace Arc::Pacman
{

///////////////////////////////////////////////////////////////////////////////
Core::Core(void)
    : mGameState(nullptr)
    , mInGame(false)
{
    mGameState.reset(new Menu(true));
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
            }
        }
    }

    if (mInGame) {
        auto game = dynamic_cast<Game*>(mGameState.get());
        if (game && game->IsGameOver()) {
            while (auto event = API::PollEvent(API::Event::GAME)) {
                if (auto key = event->GetIf<API::Event::KeyPressed>()) {
                    if (key->code == EKeyboardKey::SPACE) {
                        mInGame = false;
                        mGameState->EndPlay();
                        mGameState.reset(new Menu(false));
                        mGameState->BeginPlay();
                    }
                }
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
    return ("Pac-Man (1980)");
}

///////////////////////////////////////////////////////////////////////////////
std::string Core::GetDescription(void) const
{
    return ("Pacman game");
}

///////////////////////////////////////////////////////////////////////////////
std::string Core::GetSpriteSheet(void) const
{
    return ("assets/PACMAN/sprites.png");
}

} // namespace Arc::Pacman
