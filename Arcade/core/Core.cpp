///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Arcade/core/Core.hpp"
#include "Arcade/core/Library.hpp"
#include "Arcade/core/API.hpp"
#include <chrono>

///////////////////////////////////////////////////////////////////////////////
// Namespace Arc
///////////////////////////////////////////////////////////////////////////////
namespace Arc
{

///////////////////////////////////////////////////////////////////////////////
Core::Core(const std::string& graphicLib, const std::string& gameLib)
    : mGraphics(Library::Load<IGraphicsModule>(graphicLib))
    , mIsWindowOpen(true)
{
    mStates.push(Library::Load<IGameModule>(gameLib));
    mGraphics->LoadSpriteSheet(mStates.top()->GetSpriteSheet());
}

///////////////////////////////////////////////////////////////////////////////
void Core::HandleEvents(void)
{
    static std::vector<std::string> GRAPHICS = {
        "lib/arcade_sfml.so",
        "lib/arcade_sdl2.so",
        "lib/arcade_ncurses.so"
    };
    static std::vector<std::string> GAMES = {
        "lib/arcade_nibbler.so",
        "lib/arcade_pacman.so"
    };

    while (std::optional event = API::PollEvent(API::Event::CORE)) {
        if (auto change = event->GetIf<API::Event::ChangeGraphics>()) {
            mGraphicLibIdx += change->delta;

            if (mGraphicLibIdx < 0) {
                mGraphicLibIdx = GRAPHICS.size() - 1;
            } else if (mGraphicLibIdx >= (int)GRAPHICS.size()) {
                mGraphicLibIdx = 0;
            }

            auto currentGame = mStates.top();

            mGraphics.reset();

            try {
                mGraphics = Library::Load<IGraphicsModule>(GRAPHICS[mGraphicLibIdx]);
                mGraphics->LoadSpriteSheet(currentGame->GetSpriteSheet());
                mGraphics->SetTitle(currentGame->GetName());

                currentGame->BeginPlay();
            } catch (const std::exception& e) {
                std::cerr << "Failed to load graphics library: " << e.what() << std::endl;
            }
        } else if (auto change = event->GetIf<API::Event::ChangeGame>()) {
            mGameLibIdx += change->delta;

            if (mGameLibIdx < 0) {
                mGameLibIdx = GAMES.size() - 1;
            } else if (mGameLibIdx >= (int)GAMES.size()) {
                mGameLibIdx = 0;
            }

            mStates.top()->EndPlay();

            if (mStates.top()->GetName() != "GUI") {
                mStates.pop();
            }

            mStates.push(Library::Load<IGameModule>(GAMES[mGameLibIdx]));
            mGraphics->LoadSpriteSheet(mStates.top()->GetSpriteSheet());

            mGraphics->SetTitle(mStates.top()->GetName());

            mStates.top()->BeginPlay();
        } else if (event->Is<API::Event::Closed>()) {
            mIsWindowOpen = false;
        } else if (auto key = event->GetIf<API::Event::KeyPressed>()) {
            switch (key->code) {
                case EKeyboardKey::O:
                    API::PushEvent(API::Event::Channel::CORE,
                        API::Event::ChangeGraphics{-1});
                    break;
                case EKeyboardKey::P:
                    API::PushEvent(API::Event::Channel::CORE,
                        API::Event::ChangeGraphics{1});
                    break;
                case EKeyboardKey::L:
                    API::PushEvent(API::Event::Channel::CORE,
                        API::Event::ChangeGame{-1});
                    break;
                case EKeyboardKey::M:
                    API::PushEvent(API::Event::Channel::CORE,
                        API::Event::ChangeGame{1});
                    break;
                default:
                    API::PushEvent(API::Event::Channel::GAME,
                        API::Event::KeyPressed{key->code});
                    break;
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void Core::Run(void)
{
    auto start = std::chrono::high_resolution_clock::now();

    if (mStates.size() == 0) {
        return;
    }

    mGraphics->SetTitle(mStates.top()->GetName());

    mStates.top()->BeginPlay();
    while (mIsWindowOpen && mStates.size() > 0) {
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> duration = end - start;
        start = end;

        HandleEvents();
        mGraphics->Update();
        mGraphics->Clear();
        mStates.top()->Tick(duration.count());
        mGraphics->Render();
    }
    mStates.top()->EndPlay();
}

} // namespace Arc
