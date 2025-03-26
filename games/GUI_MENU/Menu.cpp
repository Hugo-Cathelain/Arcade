///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "games/GUI_MENU/Menu.hpp"
#include "games/GUI_MENU/Assets.hpp"
#include "Arcade/core/API.hpp"
#include <iostream>
#include <string>

///////////////////////////////////////////////////////////////////////////////
// Namespace Arc
///////////////////////////////////////////////////////////////////////////////
namespace Arc
{

///////////////////////////////////////////////////////////////////////////////
MenuGUI::MenuGUI(void)
    : mGameOver(false)
    , mCurrentGame(0)
{}

///////////////////////////////////////////////////////////////////////////////
MenuGUI::~MenuGUI()
{}

///////////////////////////////////////////////////////////////////////////////
void MenuGUI::Write(const std::string& text, const Vec2i& position)
{
    for (size_t i = 0; i < text.size(); ++i) {
        for (size_t j = 0; j < CHARACTERS.size(); ++j) {
            if (text[i] == CHARACTERS[j]) {
                int ji = static_cast<int>(j);
                API::Draw(
                    IGameModule::Asset(
                        {ji % GRID_SIZE, ji / GRID_SIZE},
                        std::string(1, text[i])
                    ),
                    Vec2i(position.x + static_cast<int>(i), position.y)
                );
                break;
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void MenuGUI::BeginPlay(void)
{
    API::PushEvent(API::Event::GRAPHICS, API::Event::GridSize({42, 32}));
}

///////////////////////////////////////////////////////////////////////////////
void MenuGUI::EndPlay(void)
{}

///////////////////////////////////////////////////////////////////////////////
void MenuGUI::Tick(float deltaSeconds)
{
    (void)deltaSeconds;

    while (auto event = API::PollEvent(API::Event::GAME)) {
        if (auto key = event->GetIf<API::Event::KeyPressed>()) {
            if (key->code == EKeyboardKey::ESCAPE) {
                API::PushEvent(API::Event::CORE, API::Event::Closed{});
            } else if (key->code == EKeyboardKey::SPACE) {
                API::PushEvent(API::Event::CORE, API::Event::ChangeGame{+1});
            } else if (key->code == EKeyboardKey::LEFT) {
                mCurrentGame--;
                if (mCurrentGame < 0) {
                    mCurrentGame = 3;
                }
            } else if (key->code == EKeyboardKey::RIGHT) {
                mCurrentGame++;
                if (mCurrentGame > 3) {
                    mCurrentGame = 0;
                }
            }
        }
    }

    API::Draw(
        IGameModule::Asset({0, 11}, "ARCADE", CLR_WHITE, {128, 40}),
        Vec2i(21, 2)
    );

    API::Draw(
        IGameModule::Asset(
            {0, 16 + 8 * mCurrentGame},
            "NIBBLER", CLR_WHITE,
            {256, 64}
        ),
        Vec2i(21, 16)
    );
}

///////////////////////////////////////////////////////////////////////////////
bool MenuGUI::IsGameOver(void) const
{
    return (mGameOver);
}

///////////////////////////////////////////////////////////////////////////////
int MenuGUI::GetScore(void) const
{
    return (0);
}

///////////////////////////////////////////////////////////////////////////////
std::string MenuGUI::GetName(void) const
{
    return ("GUI");
}

///////////////////////////////////////////////////////////////////////////////
std::string MenuGUI::GetDescription(void) const
{
    return ("MenuGUI");
}

///////////////////////////////////////////////////////////////////////////////
std::string MenuGUI::GetSpriteSheet(void) const
{
    return ("assets/GUI/sprites.png");
}

} // namespace Arc
