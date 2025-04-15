///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Arcade/core/Core.hpp"
#include "Arcade/core/Library.hpp"
#include "Arcade/core/API.hpp"
#include "Arcade/shared/Joystick.hpp"
#include "Arcade/audio/Audio.hpp"
#include "Arcade/shared/WiiMote.hpp"
#include "Arcade/errors/Exception.hpp"
#include <chrono>
#include <filesystem>
#include <fstream>

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
    : mIsWindowOpen(true)
    , mTimer(0.f)
{
    SetLibraries(graphicLib, gameLib);

    mGraphics->LoadSpriteSheet(mStates.top()->GetSpriteSheet());

    WiiMote::Initialize();

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
void Core::SetLibraries(
    const std::string& graphicLib,
    const std::string& gameLib
)
{
    if (!Library::Is<IGraphicsModule>(graphicLib)) {
        mGraphicLib = "lib/arcade_sfml.so";
        std::cerr << "ERROR: Invalid Graphics libraries, using SFML instead."
                  << std::endl;
    } else {
        mGraphicLib = graphicLib;
    }

    if (
        !Library::Is<IGameModule>(gameLib) &&
        gameLib != "lib/arcade_gui_menu.so"
    ) {
        std::cerr << "ERROR: Invalid Game libraries, using MenuGUI instead."
                  << std::endl;
        mGameLib = "lib/arcade_gui_menu.so";
    } else {
        mGameLib = gameLib;
    }

    mGraphics = Library::Load<IGraphicsModule>(mGraphicLib);
    mStates.push(Library::Load<IGameModule>(mGameLib));
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
void Core::SaveScore(int score)
{
    if (mUserName.empty()) {
        return;
    }

    if (!fs::exists(".saves")) {
        fs::create_directory(".saves");
    }

    std::string savePath = ".saves/" + mUserName + ".save";
    std::string gameName = mStates.top()->GetName();
    int currentScore = score;

    std::map<std::string, int> scores;

    std::ifstream saveFile(savePath);
    if (saveFile.is_open()) {
        std::string line;
        while (std::getline(saveFile, line)) {
            size_t sep = line.find(':');
            if (sep != std::string::npos) {
                std::string game = line.substr(0, sep);
                int score = std::stoi(line.substr(sep + 1));
                scores[game] = score;
            }
        }
        saveFile.close();
    }

    if (
        scores.find(gameName) == scores.end() ||
        currentScore > scores[gameName]
    ) {
        scores[gameName] = currentScore;
    }

    std::ofstream outFile(savePath);
    if (outFile.is_open()) {
        for (const auto& [game, score] : scores) {
            outFile << game << ":" << score << std::endl;
        }
        outFile.close();
    } else {
        std::cerr << "Failed to open save file for writing: "
                  << savePath << std::endl;
    }
}

///////////////////////////////////////////////////////////////////////////////
void Core::SetGame(const std::string& game)
{
    std::string path = "";
    for (const auto& [p, name] : mGameLibs) {
        if (name == game) {
            path = p;
            break;
        }
    }
    if (path.empty()) {
        return;
    }

    mGameLib = path;
    if (mStates.top()->GetName() != "MenuGUI") {
        mStates.top()->EndPlay();
        mStates.pop();
    }
    mStates.push(Library::Load<IGameModule>(path));
    mGraphics->LoadSpriteSheet(mStates.top()->GetSpriteSheet());
    mGraphics->SetTitle(mStates.top()->GetName());
    mStates.top()->BeginPlay();
}

///////////////////////////////////////////////////////////////////////////////
void Core::SetGraphics(const std::string& graphics)
{
    std::string path = "";
    for (const auto& [p, name] : mGraphicLibs) {
        if (name == graphics) {
            path = p;
            break;
        }
    }
    if (path.empty()) {
        return;
    }

    mGraphicLib = path;
    mGraphics.reset();
    try {
        mGraphics = Library::Load<IGraphicsModule>(path);
        mGraphics->LoadSpriteSheet(mStates.top()->GetSpriteSheet());
        mGraphics->SetTitle(mStates.top()->GetName());
        mStates.top()->BeginPlay();
    } catch (const std::exception& e) {
        std::cerr << "Failed to load graphics library: "
                  << e.what() << std::endl;
    }
}

///////////////////////////////////////////////////////////////////////////////
void Core::HandleKeyPressed(EKeyboardKey code)
{
    if (mStates.top()->GetName() == "MenuGUI") {
        API::PushEvent(API::Event::Channel::GAME,
            API::Event::KeyPressed{code}
        );
        return;
    }
    switch (code) {
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
        {
            if (mStates.size() > 1) {
                mStates.top()->EndPlay();
                mStates.pop();
                mGraphics->LoadSpriteSheet(
                    mStates.top()->GetSpriteSheet()
                );
                mGraphics->SetTitle(mStates.top()->GetName());
                mStates.top()->BeginPlay();
            }
            break;
        }
        case EKeyboardKey::W:
            if (WiiMote::Find()) {
                WiiMote::Connect();
            }
            break;
        default:
            API::PushEvent(API::Event::Channel::GAME,
                API::Event::KeyPressed{code}
            );
            break;
    }
}

///////////////////////////////////////////////////////////////////////////////
void Core::HandleGraphicsRotation(int delta)
{
    std::vector<std::string> libs;

    for (const auto& [key, value] : mGraphicLibs) {
        libs.push_back(key);
    }

    mGraphicLibIdx += delta;

    if (mGraphicLibIdx < 0) {
        mGraphicLibIdx = libs.size() - 1;
    } else if (mGraphicLibIdx >= (int)libs.size()) {
        mGraphicLibIdx = 0;
    }

    auto currentGame = mStates.top();

    mGraphics.reset();

    try {
        mGraphics = Library::Load<IGraphicsModule>(
            libs[mGraphicLibIdx]
        );
        mGraphics->LoadSpriteSheet(currentGame->GetSpriteSheet());
        mGraphics->SetTitle(currentGame->GetName());

        currentGame->BeginPlay();
    } catch (const std::exception& e) {
        std::cerr << "Failed to load graphics library: "
                  << e.what() << std::endl;
    }
}

///////////////////////////////////////////////////////////////////////////////
void Core::HandleGameRotation(int delta)
{
    std::vector<std::string> libs;

    for (const auto& [key, value] : mGameLibs) {
        libs.push_back(key);
    }

    API::PushEvent(API::Event::Channel::GRAPHICS, API::Event::ChangeGame{0});

    mGameLibIdx += delta;

    if (mGameLibIdx < 0) {
        mGameLibIdx = libs.size() - 1;
    } else if (mGameLibIdx >= (int)libs.size()) {
        mGameLibIdx = 0;
    }

    Audio::StopAll();

    if (mStates.top()->GetName() != "MenuGUI") {
        mStates.top()->EndPlay();
        mStates.pop();
    }

    mStates.push(Library::Load<IGameModule>(libs[mGameLibIdx]));
    mGraphics->LoadSpriteSheet(mStates.top()->GetSpriteSheet());

    mGraphics->SetTitle(mStates.top()->GetName());

    mStates.top()->BeginPlay();
}

///////////////////////////////////////////////////////////////////////////////
void Core::HandleEvents(void)
{
    while (std::optional event = API::PollEvent(API::Event::CORE)) {
        if (auto change = event->GetIf<API::Event::ChangeGraphics>()) {
            HandleGraphicsRotation(change->delta);
        } else if (auto change = event->GetIf<API::Event::ChangeGame>()) {
            HandleGameRotation(change->delta);
        } else if (event->Is<API::Event::Closed>()) {
            mIsWindowOpen = false;
        } else if (auto key = event->GetIf<API::Event::KeyPressed>()) {
            HandleKeyPressed(key->code);
        } else if (auto lib = event->GetIf<API::Event::SetGame>()) {
            SetGame(lib->game);
        } else if (auto lib = event->GetIf<API::Event::SetGraphics>()) {
            SetGraphics(lib->graphical);
        } else if (auto info = event->GetIf<API::Event::PlayerInformation>()) {
            if (!info->username.empty()) {
                mUserName = info->username;
            }
        } else if (auto over = event->GetIf<API::Event::GameOver>()) {
            SaveScore(over->score);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
int Core::IsAxisPressed(Joystick::Axis axis)
{
    float value = Joystick::GetAxisPosition(0, axis);
    float absValue = std::abs(value);

    if (mAxisPressed[axis] && absValue < 70.f) {
        mAxisPressed.erase(axis);
        return (0);
    }

    if (mAxisPressed[axis]) {
        return (0);
    }

    if (absValue > 70.f) {
        mAxisPressed[axis] = true;
        return (value < 0.f ? -1 : 1);
    }
    return (0);
}

///////////////////////////////////////////////////////////////////////////////
void Core::HandleJoystick(void)
{
    if (Joystick::IsButtonPressed(0, 0)) {
        API::PushEvent(API::Event::Channel::GAME, API::Event::KeyPressed{
            EKeyboardKey::SPACE
        });
    }

    if (Joystick::IsButtonPressed(0, 7)) {
        API::PushEvent(API::Event::Channel::GAME, API::Event::KeyPressed{
            EKeyboardKey::Q
        });
    }

    if (auto delta = IsAxisPressed(Joystick::Axis::PovX)) {
        API::PushEvent(API::Event::Channel::GAME, API::Event::KeyPressed{
            delta < 0 ? EKeyboardKey::LEFT : EKeyboardKey::RIGHT
        });
    } else if (auto delta = IsAxisPressed(Joystick::Axis::X)) {
        API::PushEvent(API::Event::Channel::GAME, API::Event::KeyPressed{
            delta < 0 ? EKeyboardKey::LEFT : EKeyboardKey::RIGHT
        });
    } else if (auto delta = IsAxisPressed(Joystick::Axis::U)) {
        API::PushEvent(API::Event::Channel::GAME, API::Event::KeyPressed{
            delta < 0 ? EKeyboardKey::LEFT : EKeyboardKey::RIGHT
        });
    }

    if (auto delta = IsAxisPressed(Joystick::Axis::PovY)) {
        API::PushEvent(API::Event::Channel::GAME, API::Event::KeyPressed{
            delta < 0 ? EKeyboardKey::UP : EKeyboardKey::DOWN
        });
    } else if (auto delta = IsAxisPressed(Joystick::Axis::Y)) {
        API::PushEvent(API::Event::Channel::GAME, API::Event::KeyPressed{
            delta < 0 ? EKeyboardKey::UP : EKeyboardKey::DOWN
        });
    } else if (auto delta = IsAxisPressed(Joystick::Axis::V)) {
        API::PushEvent(API::Event::Channel::GAME, API::Event::KeyPressed{
            delta < 0 ? EKeyboardKey::UP : EKeyboardKey::DOWN
        });
    }
}

///////////////////////////////////////////////////////////////////////////////
void Core::HandleWiiMote(void)
{
    WiiMote::Update();

    WiiMote::Accelerometer accel = WiiMote::GetAccelerometer(0);

    if (accel.pitch > 50) {
        API::PushEvent(
            API::Event::Channel::GAME,
            API::Event::KeyPressed{EKeyboardKey::LEFT}
        );
    } else if (accel.pitch < -50) {
        API::PushEvent(
            API::Event::Channel::GAME,
            API::Event::KeyPressed{EKeyboardKey::RIGHT}
        );
    } else if (accel.roll > 0 && accel.roll < 50) {
        API::PushEvent(
            API::Event::Channel::GAME,
            API::Event::KeyPressed{EKeyboardKey::DOWN}
        );
    } else if (accel.roll < -130) {
        API::PushEvent(
            API::Event::Channel::GAME,
            API::Event::KeyPressed{EKeyboardKey::UP}
        );
    }

    if (WiiMote::IsButtonPressed(0, WiiMote::Button::A)) {
        if (!mButtonPressed[WiiMote::Button::A]) {
            mButtonPressed[WiiMote::Button::A] = true;
            API::PushEvent(
                API::Event::Channel::GAME,
                API::Event::KeyPressed{EKeyboardKey::SPACE}
            );
        }
    } else {
        mButtonPressed[WiiMote::Button::A] = false;
    }
}

///////////////////////////////////////////////////////////////////////////////
void Core::RefreshLibraries(void)
{
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

///////////////////////////////////////////////////////////////////////////////
void Core::Run(void)
{
    auto start = std::chrono::high_resolution_clock::now();

    if (mStates.size() == 0) {
        return;
    }

    Audio::Initialize();

    mGraphics->SetTitle(mStates.top()->GetName());

    mStates.top()->BeginPlay();
    while (mIsWindowOpen && mStates.size() > 0) {
        Joystick::Update();

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> duration = end - start;
        start = end;

        float deltaSeconds = duration.count();

        mTimer += deltaSeconds;

        if (mTimer >= 2.f && mStates.top()->GetName() == "MenuGUI") {
            RefreshLibraries();
        }

        if (WiiMote::IsConnected(0)) {
            HandleWiiMote();
        }

        if (Joystick::IsConnected(0)) {
            HandleJoystick();
        }
        HandleEvents();

        mGraphics->Update();
        mGraphics->Clear();
        mStates.top()->Tick(deltaSeconds);
        mGraphics->Render();
    }
    mStates.top()->EndPlay();

    WiiMote::Cleanup();

    Audio::Shutdown();
}

} // namespace Arc
