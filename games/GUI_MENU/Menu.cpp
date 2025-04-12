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
    , mTimer(0.f)
    , mUserNameSelected(false)
{}

///////////////////////////////////////////////////////////////////////////////
MenuGUI::~MenuGUI()
{}

///////////////////////////////////////////////////////////////////////////////
void MenuGUI::Text(
    const std::string& text,
    MenuGUI::TextColor color,
    const Vec2i& position
)
{
    static const int LTRS_OFFSET_X = 107;
    static const char LTRS[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ!@..0123456789/-\"";
    static const int LTRS_ROW_SIZE = 15;
    static const Color LTS_COLORS[] = {
        Color{224, 221, 255}, Color{255, 0, 0}, Color{252, 181, 255},
        Color{0, 255, 255}, Color{248, 187, 85}, Color{250, 185, 176},
        Color{255, 255, 0}, Color{0, 255, 0}
    };

    for (size_t i = 0; i < text.size(); i++) {
        int index = -1;
        for (size_t j = 0; j < sizeof(LTRS); j++) {
            if (text[i] == LTRS[j]) {
                index = j;
                break;
            }
        }

        if (index == -1) {
            continue;
        }

        int row = index / LTRS_ROW_SIZE + static_cast<int>(color) * 4;
        int col = index % LTRS_ROW_SIZE;

        IGameModule::Asset sprite(
            {LTRS_OFFSET_X + col, row},
            std::string(1, text[i]),
            LTS_COLORS[static_cast<int>(color)]
        );

        API::Draw(sprite, position + Vec2i{static_cast<int>(i), 0});
    }
}

///////////////////////////////////////////////////////////////////////////////
void MenuGUI::HandleEvents(void)
{
    while (auto event = API::PollEvent(API::Event::GAME)) {
        if (auto key = event->GetIf<API::Event::KeyPressed>()) {
            if (key->code == EKeyboardKey::SPACE) {
                if (!mUserNameSelected) {
                    mUserNameSelected = true;
                    API::PushEvent(
                        API::Event::CORE,
                        API::Event::PlayerInformation({mUserName})
                    );
                } else {
                    API::PushEvent(
                        API::Event::CORE,
                        API::Event::SetGame{mGames[mCurrentGame]}
                    );
                }
            } else if (key->code == EKeyboardKey::LEFT) {
                mCurrentGame--;
                if (mCurrentGame < 0) {
                    mCurrentGame = static_cast<int>(mGames.size() - 1);
                }
            } else if (key->code == EKeyboardKey::RIGHT) {
                mCurrentGame++;
                if (mCurrentGame > static_cast<int>(mGames.size() - 1)) {
                    mCurrentGame = 0;
                }
            } else if (
                key->code >= EKeyboardKey::A &&
                key->code <= EKeyboardKey::Z &&
                mUserName.length() < 10
            ) {
                mUserName += ('A' +
                    static_cast<int>(key->code) -
                    static_cast<int>(EKeyboardKey::A)
                );
            }
        } else if (auto libraries = event->GetIf<API::Event::Libraries>()) {
            mGames = libraries->games;
            mGraphicals = libraries->graphicals;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void MenuGUI::DrawBootAnimation(void)
{
    static const std::vector<std::string> checks = {
        "PREPARING ARCADE",
        "PREPARING GUI",
        "PROJECT ARCADE 2025",
        "MADE FOR EPITECH",
        "AUDIO SYSTEM",
        "CONTROLLER SUPPORT",
        "WIIMOTE SUPPORT",
        "LOADING CORE",
        "FETCHING GRAPHICALS",
        "FETCHING GAMES",
        "INIT BUFFERS",
        "INIT CORE",
        "INIT BLUETHOOTH",
        "INIT GRAPHICALS",
        "INIT GAMES",
        "PREPARING SPRITES",
        "PREPARING AUDIO",
        "PREPARING CONTROLLERS",
        "PREPARING WIIMOTE",
        "CHECKING MEMORY DUMP",
        "CHECKING MEMORY",
        "CHECKING CONNECTION",
        "CHECKING LIBRARIES",
        "FETCHING FOR UPDATES",
        "LOADING MAIN MENU",
        "CHECK FOR INPUTS",
        "SAVING AXOLOTL",
        "AXOLOTL GOT AN AK47",
        "FOOD AS BEEN STOLEN",
        "AXOLOTL IS ANGRY",
        "AXOLOTL SHOOT THE TURLE",
        "AXOLOTL IS HAPPY",
        "AXOLOTL IS A HERO",
        "AXOLOTL IS A LEGEND",
        "AXOLOTL IS A GOD",
        "AXOLOTL IS A DEMIGOD",
        "AXOLOTL IS A DEMON",
        "AXOLOTL IS A DEVIL",
        "OH! SEGMENTATION FAULT",
        "MADE BY MALLOW",
        "MADE BY HUGO",
        "MADE BY NATHAN",
        "FEATURING OSSAN",
        "FEATURING RAPHAEL",
        "FEATURING FOUDIL",
        "LAUNCHING MENU"
    };
    float progress = mTimer / 4.f;

    if (mTimer > 4.f) {
        progress = 1.f;
    }

    int max = static_cast<int>(progress * checks.size());
    int offset = max - 36;

    for (int i = 0; i < max; i++) {
        int y = i;
        if (offset > 0) {
            y -= offset;
        }

        Text("-    - " + checks[i], TextColor::TEXT_WHITE, Vec2i({0, y}));
        if (i != max - 1 || mTimer > 4.f) {
            Text("OK", TextColor::TEXT_GREEN, Vec2i({2, y}));
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void MenuGUI::DrawUsernameSelection(void)
{
    bool flick = static_cast<int>(mTimer) % 2 ? true : false;

    Text(mUserName, TextColor::TEXT_WHITE, Vec2i({3, 16}));

    if (flick) {
        Text(
            "-", TextColor::TEXT_WHITE,
            Vec2i({3 + static_cast<int>(mUserName.size()), 16})
        );
    }
}

///////////////////////////////////////////////////////////////////////////////
void MenuGUI::DrawGameSelection(void)
{
    IGameModule::Asset gamePoster = SPRITES[GAME_UNKNOWN];

    if (SPRITE_MAP.count(mGames[mCurrentGame])) {
        gamePoster = SPRITES[SPRITE_MAP.at(mGames[mCurrentGame])];
    }
    API::Draw(gamePoster, Vec2i({15, 16}));
    Text(mGames[mCurrentGame], TextColor::TEXT_WHITE, Vec2i({3, 34}));

    bool flick = static_cast<int>(mTimer) % 2 ? true : false;

    if (flick) {
        Text("PRESS START", TextColor::TEXT_PINK, Vec2i({17, 34}));
    }
}

///////////////////////////////////////////////////////////////////////////////
void MenuGUI::BeginPlay(void)
{
    API::PushEvent(API::Event::GRAPHICS, API::Event::GridSize({30, 36}));
}

///////////////////////////////////////////////////////////////////////////////
void MenuGUI::EndPlay(void)
{}

///////////////////////////////////////////////////////////////////////////////
void MenuGUI::Tick(float deltaSeconds)
{
    mTimer += deltaSeconds;

    HandleEvents();

    if (mTimer < 5.f) {
        DrawBootAnimation();
    } else if (!mUserNameSelected) {
        DrawUsernameSelection();
    } else {
        DrawGameSelection();
    }
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
    return ("MenuGUI");
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
