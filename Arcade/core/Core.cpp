///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Arcade/core/Core.hpp"
#include "Arcade/core/Library.hpp"
#include "Arcade/core/API.hpp"
#include <chrono>
#include <filesystem>

///////////////////////////////////////////////////////////////////////////////
// Forward namespace std::filesystem
///////////////////////////////////////////////////////////////////////////////
namespace fs = std::filesystem;

///////////////////////////////////////////////////////////////////////////////
// Namespace Arc
///////////////////////////////////////////////////////////////////////////////
namespace Arc
{

///////////////////////////////////////////////////////////////////////////////
Core::Core(const std::string& graphicLib, const std::string& gameLib)
    : mGraphics(Library::Load<IGraphicsModule>(graphicLib))
    , mIsWindowOpen(true)
    , mTimer(0.f)
{
    mStates.push(Library::Load<IGameModule>(gameLib));
    mGraphics->LoadSpriteSheet(mStates.top()->GetSpriteSheet());

    GetLibraries();

    std::vector<std::string> games, graphicals;

    for (const auto& [path, name] : mGameLibs) {
        games.push_back(name);
    }
    for (const auto& [path, name] : mGraphicLibs) {
        graphicals.push_back(name);
    }

    API::PushEvent(API::Event::GAME, API::Event::Libraries{graphicals, games});
}

///////////////////////////////////////////////////////////////////////////////
void Core::GetLibraries(void)
{
    mGraphicLibs.clear();
    mGameLibs.clear();

    try {
        for (const auto& entry : fs::directory_iterator("lib")) {
            if (!entry.is_regular_file() || entry.path().extension() != ".so") {
                continue;
            }

            std::string path = entry.path().string();

            if (auto name = Library::Is<IGraphicsModule>(path)) {
                mGraphicLibs[path] = name.value();
            } else if (auto name = Library::Is<IGameModule>(path)) {
                mGameLibs[path] = name.value();
            }
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Error accessing directory: " << e.what() << std::endl;
    }
}

///////////////////////////////////////////////////////////////////////////////
void Core::HandleEvents(void)
{
    static std::vector<std::string> GRAPHICS = {
        "lib/arcade_sfml.so",
        "lib/arcade_sdl2.so",
        "lib/arcade_opengl.so",
        "lib/arcade_ncurses.so"
    };
    static std::vector<std::string> GAMES = {
        "lib/arcade_snake.so",
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
            API::PushEvent(API::Event::Channel::GRAPHICS,
                API::Event::ChangeGame{0});

            mGameLibIdx += change->delta;

            if (mGameLibIdx < 0) {
                mGameLibIdx = GAMES.size() - 1;
            } else if (mGameLibIdx >= (int)GAMES.size()) {
                mGameLibIdx = 0;
            }

            mStates.top()->EndPlay();

            if (mStates.top()->GetName().find_first_of("GUI") == std::string::npos) {
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
                case EKeyboardKey::R:
                    API::PushEvent(API::Event::Channel::CORE,
                        API::Event::ChangeGame{0});
                    break;
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
                case EKeyboardKey::Q:
                    API::PushEvent(API::Event::Channel::CORE,
                        API::Event::Closed{});
                    break;
                case EKeyboardKey::F:
                    GetLibraries();
                    break;
                default:
                    API::PushEvent(API::Event::Channel::GAME,
                        API::Event::KeyPressed{key->code});
                    break;
            }
        } else if (auto lib = event->GetIf<API::Event::SetGame>()) {
            std::string path = "";
            for (const auto& [p, name] : mGameLibs) {
                if (name == lib->game) {
                    path = p;
                    break;
                }
            }
            if (path.empty()) {
                continue;
            }

            mGameLib = path;
            mStates.top()->EndPlay();
            mStates.pop();
            mStates.push(Library::Load<IGameModule>(path));
            mGraphics->LoadSpriteSheet(mStates.top()->GetSpriteSheet());
            mGraphics->SetTitle(mStates.top()->GetName());
            mStates.top()->BeginPlay();
        } else if (auto lib = event->GetIf<API::Event::SetGraphics>()) {
            std::string path = "";
            for (const auto& [p, name] : mGraphicLibs) {
                if (name == lib->graphical) {
                    path = p;
                    break;
                }
            }
            if (path.empty()) {
                continue;
            }

            mGraphicLib = path;
            mGraphics.reset();
            try {
                mGraphics = Library::Load<IGraphicsModule>(path);
                mGraphics->LoadSpriteSheet(mStates.top()->GetSpriteSheet());
                mGraphics->SetTitle(mStates.top()->GetName());
                mStates.top()->BeginPlay();
            } catch (const std::exception& e) {
                std::cerr << "Failed to load graphics library: " << e.what() << std::endl;
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

        float deltaSeconds = duration.count();

        mTimer += deltaSeconds;

        if (mTimer >= 2.f && mStates.top()->GetName() == "MenuGUI") {
            mTimer = 0.f;
            GetLibraries();
            std::vector<std::string> games, graphicals;

            for (const auto& [path, name] : mGameLibs) {
                games.push_back(name);
            }
            for (const auto& [path, name] : mGraphicLibs) {
                graphicals.push_back(name);
            }

            API::PushEvent(
                API::Event::GAME,
                API::Event::Libraries{graphicals, games}
            );
        }

        HandleEvents();
        mGraphics->Update();
        mGraphics->Clear();
        mStates.top()->Tick(deltaSeconds);
        mGraphics->Render();
    }
    mStates.top()->EndPlay();
}

} // namespace Arc
