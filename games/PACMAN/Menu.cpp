///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "games/PACMAN/Menu.hpp"
#include "games/PACMAN/Assets.hpp"
#include "../../Arcade/interfaces/IGameModule.hpp"
#include "../../Arcade/core/API.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Arc::Pacman
///////////////////////////////////////////////////////////////////////////////
namespace Arc::Pacman
{

///////////////////////////////////////////////////////////////////////////////
Menu::Menu(void)
    : mTimer(0.f)
    , mState(State::ROM_CHECKSUM)
{}

///////////////////////////////////////////////////////////////////////////////
Menu::~Menu()
{}

///////////////////////////////////////////////////////////////////////////////
void Menu::Text(
    const std::string& text,
    Menu::TextColor color,
    const Vec2i& position
)
{
    static const int LTRS_OFFSET_Y = 10;
    static const char LTRS[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ!@..0123456789/-\"";
    static const int LTRS_ROW_SIZE = 15;
    static const Color LTS_COLORS[] = {
        Color{224, 221, 255}, Color{255, 0, 0}, Color{252, 181, 255},
        Color{0, 255, 255}, Color{248, 187, 85}, Color{250, 185, 176},
        Color{255, 255, 0}
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
            {col, LTRS_OFFSET_Y + row},
            std::string(1, text[i]),
            LTS_COLORS[static_cast<int>(color)]
        );

        API::Draw(sprite, position + Vec2i{static_cast<int>(i), 0});
    }
}

///////////////////////////////////////////////////////////////////////////////
void Menu::BeginPlay(void)
{}

///////////////////////////////////////////////////////////////////////////////
void Menu::DrawRomChecksum(void)
{
    static const std::string FLICK_TEXT[5] = {
        "1B5F93D71B5F93D7", "E82C60A4E82C60A4", "DF13579BDF13579B",
        "E02468ACE02468AC", "EBFC966630DA741E"
    };

    for (int y = 0; y < ARCADE_SCREEN_HEIGHT; y++) {
        std::string text = FLICK_TEXT[(y + static_cast<int>(mTimer * 50)) % 5];
        Text(text + text + text + text, TextColor::TEXT_WHITE, Vec2i{0, y});
    }

    if (mTimer > 1.0f) {
        mState = State::MENU;
        mTimer = 0.0f;

        for (int i = 0; i < ARCADE_SCREEN_WIDTH; i++) {
            for (int j = 0; j < ARCADE_SCREEN_HEIGHT; j++) {
                API::Draw(IGameModule::Asset({15, 15}, "  "), Vec2i{i, j});
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
float Menu::Lerp(float start, float end, float duration, float current)
{
    return (start - (current / duration) * (start - end));
}

///////////////////////////////////////////////////////////////////////////////
void Menu::DrawMenuTextStatic(void)
{
    Text("1UP   HIGH SCORE   2UP", TextColor::TEXT_WHITE, Vec2i{3, 0});
    Text("00", TextColor::TEXT_WHITE, Vec2i{5, 1});
    Text("CHARACTER / NICKNAME", TextColor::TEXT_WHITE, Vec2i{7, 4});
    Text("CREDIT  0", TextColor::TEXT_WHITE, Vec2i{3, 35});
}

///////////////////////////////////////////////////////////////////////////////
void Menu::DrawMenuTextAnimated(void)
{
    if (mTimer > 1.0f) {
        auto blinky = SPRITES[RED_R1];
        blinky.id = -1;
        API::Draw(blinky, Vec2i{4, 6});
    }
    if (mTimer > 2.0f) {
        Text("-SHADOW", TextColor::TEXT_RED, Vec2i{7, 6});
    }
    if (mTimer > 2.5f) {
        Text("\"BLINKY\"", TextColor::TEXT_RED, Vec2i{18, 6});
    }

    if (mTimer > 3.0f) {
        auto pinky = SPRITES[PINK_R1];
        pinky.id = -1;
        API::Draw(pinky, Vec2i{4, 9});
    }
    if (mTimer > 4.0f) {
        Text("-SPEEDY", TextColor::TEXT_PINK, Vec2i{7, 9});
    }
    if (mTimer > 4.5f) {
        Text("\"PINKY\"", TextColor::TEXT_PINK, Vec2i{18, 9});
    }

    if (mTimer > 5.0f) {
        auto inky = SPRITES[CYAN_R1];
        inky.id = -1;
        API::Draw(inky, Vec2i{4, 12});
    }
    if (mTimer > 6.0f) {
        Text("-BASHFUL", TextColor::TEXT_CYAN, Vec2i{7, 12});
    }
    if (mTimer > 6.5f) {
        Text("\"INKY\"", TextColor::TEXT_CYAN, Vec2i{18, 12});
    }

    if (mTimer > 7.0f) {
        auto clyde = SPRITES[ORANGE_R1];
        clyde.id = -1;
        API::Draw(clyde, Vec2i{4, 15});
    }
    if (mTimer > 8.0f) {
        Text("-POKEY", TextColor::TEXT_ORANGE, Vec2i{7, 15});
    }
    if (mTimer > 8.5f) {
        Text("\"CLYDE\"", TextColor::TEXT_ORANGE, Vec2i{18, 15});
    }

    if (mTimer > 10.0f) {
        API::Draw(SPRITES[TILE_POINT], Vec2i{10, 24});
        Text("10", TextColor::TEXT_WHITE, Vec2i{12, 24});
        API::Draw(SPRITES[WHITE_PTS], Vec2i{16, 24});
        if (mTimer < 12.f) {
            API::Draw(SPRITES[TILE_PACGUM], Vec2i{10, 26});
        }
        Text("50", TextColor::TEXT_WHITE, Vec2i{12, 26});
        API::Draw(SPRITES[WHITE_PTS], Vec2i{16, 26});
    }

    if (mTimer > 11.0f) {
        Text("@ 1980 NAMCO TKD", TextColor::TEXT_WHITE, Vec2i{6, 29});
        API::Draw(SPRITES[NAMCO_LOGO], Vec2i{14, 31});
        if (mTimer < 12.f) {
            API::Draw(SPRITES[TILE_PACGUM], Vec2i{4, 20});
        }
    }

    int pacgumFlick = static_cast<int>(mTimer * 4) % 2;
    int flick = static_cast<int>(mTimer * 8) % 2 ? 1 : 0;

    static int lastGhostX = 0;

    if (mTimer > 12.f && pacgumFlick == 0) {
        API::Draw(SPRITES[TILE_PACGUM], Vec2i{10, 26});
        if (mTimer < 15.5f) {
            API::Draw(SPRITES[TILE_PACGUM], Vec2i{4, 20});
        }
    } else {
        API::Draw(SPRITES[TILE_EMPTY], Vec2i{10, 26});
        API::Draw(SPRITES[TILE_EMPTY], Vec2i{4, 20});
    }

    if (mTimer > 12.f && mTimer < 15.5f) {
        float pacmanX = Lerp(30.f, 4.f, 3.5f, mTimer - 12.0f);
        int ghostX = static_cast<int>(Lerp(33.f, 6.f, 3.5f, mTimer - 12.0f));

        API::Draw(SPRITES[TILE_EMPTY], Vec2i{static_cast<int>(pacmanX) + 1, 20});
        API::Draw(PACMAN_XY(2, flick * 2),
        Vec2i{static_cast<int>(pacmanX), 20});

        API::Draw(SPRITES[TILE_NOTHING], Vec2i{ghostX + 1, 20});
        API::Draw(SPRITES[RED_L1 + flick], Vec2i{ghostX, 20});
        API::Draw(SPRITES[TILE_NOTHING], Vec2i{ghostX + 3, 20});
        API::Draw(SPRITES[PINK_L1 + flick], Vec2i{ghostX + 2, 20});
        API::Draw(SPRITES[TILE_NOTHING], Vec2i{ghostX + 5, 20});
        API::Draw(SPRITES[CYAN_L1 + flick], Vec2i{ghostX + 4, 20});
        API::Draw(SPRITES[TILE_NOTHING], Vec2i{ghostX + 7, 20});
        API::Draw(SPRITES[ORANGE_L1 + flick], Vec2i{ghostX + 6, 20});

        lastGhostX = ghostX;
    }

    if (mTimer > 15.5f && mTimer < 16.f) {
        float currentX = Lerp(4.f, 30.f, 3.5f, mTimer - 15.5f);
        int ghostX = static_cast<int>(Lerp(6.f, 33.f, 7.f, mTimer - 15.5f));

        API::Draw(SPRITES[TILE_EMPTY], Vec2i{static_cast<int>(currentX) - 1, 20});
        API::Draw(PACMAN_XY(
            0, flick * 2),
            Vec2i{static_cast<int>(currentX), 20}
        );

        auto ghost = SPRITES[SCARED_1 + flick];

        ghost.id = 10;
        API::Draw(SPRITES[TILE_NOTHING], Vec2i{ghostX - 1, 20});
        API::Draw(ghost, Vec2i{ghostX, 20});
        ghost.id = 11;
        API::Draw(SPRITES[TILE_NOTHING], Vec2i{ghostX + 1, 20});
        API::Draw(ghost, Vec2i{ghostX + 2, 20});
        ghost.id = 12;
        API::Draw(SPRITES[TILE_NOTHING], Vec2i{ghostX + 3, 20});
        API::Draw(ghost, Vec2i{ghostX + 4, 20});
        ghost.id = 13;
        API::Draw(SPRITES[TILE_NOTHING], Vec2i{ghostX + 5, 20});
        API::Draw(ghost, Vec2i{ghostX + 6, 20});

        lastGhostX = ghostX;
    }

    if (mTimer > 16.0f && mTimer < 18.0f) {
        API::Draw(SPRITES[SCORE_200], Vec2i{lastGhostX, 20});

        auto ghost = SPRITES[SCARED_1 + flick];

        ghost.id = 11;
        API::Draw(SPRITES[TILE_NOTHING], Vec2i{lastGhostX + 1, 20});
        API::Draw(ghost, Vec2i{lastGhostX + 2, 20});
        ghost.id = 12;
        API::Draw(SPRITES[TILE_NOTHING], Vec2i{lastGhostX + 3, 20});
        API::Draw(ghost, Vec2i{lastGhostX + 4, 20});
        ghost.id = 13;
        API::Draw(SPRITES[TILE_NOTHING], Vec2i{lastGhostX + 5, 20});
        API::Draw(ghost, Vec2i{lastGhostX + 6, 20});
    }

    if (mTimer > 18.0f && mTimer < 18.5f) {
        float currentX = Lerp(4.f, 30.f, 3.5f, mTimer - 17.5f);
        int ghostX = static_cast<int>(Lerp(6.f, 33.f, 7.f, mTimer - 17.5f));

        API::Draw(SPRITES[TILE_EMPTY], Vec2i{static_cast<int>(currentX) - 1, 20});
        API::Draw(PACMAN_XY(0, flick * 2),
            Vec2i{static_cast<int>(currentX), 20});

        auto ghost = SPRITES[SCARED_1 + flick];

        ghost.id = 11;
        API::Draw(SPRITES[TILE_NOTHING], Vec2i{ghostX + 1, 20});
        API::Draw(ghost, Vec2i{ghostX + 2, 20});
        ghost.id = 12;
        API::Draw(SPRITES[TILE_NOTHING], Vec2i{ghostX + 3, 20});
        API::Draw(ghost, Vec2i{ghostX + 4, 20});
        ghost.id = 13;
        API::Draw(SPRITES[TILE_NOTHING], Vec2i{ghostX + 5, 20});
        API::Draw(ghost, Vec2i{ghostX + 6, 20});

        lastGhostX = ghostX;
    }

    if (mTimer > 18.5f && mTimer < 20.5f) {
        API::Draw(SPRITES[SCORE_400], Vec2i{lastGhostX + 2, 20});

        auto ghost = SPRITES[SCARED_1 + flick];

        ghost.id = 12;
        API::Draw(SPRITES[TILE_NOTHING], Vec2i{lastGhostX + 3, 20});
        API::Draw(ghost, Vec2i{lastGhostX + 4, 20});
        ghost.id = 13;
        API::Draw(SPRITES[TILE_NOTHING], Vec2i{lastGhostX + 5, 20});
        API::Draw(ghost, Vec2i{lastGhostX + 6, 20});
    }

    if (mTimer > 20.5f && mTimer < 21.0f) {
        float currentX = Lerp(4.f, 30.f, 3.5f, mTimer - 19.5f);
        int ghostX = static_cast<int>(Lerp(6.f, 33.f, 7.f, mTimer - 19.5f));

        API::Draw(SPRITES[TILE_EMPTY], Vec2i{static_cast<int>(currentX) - 1, 20});
        API::Draw(PACMAN_XY(0, flick * 2),
            Vec2i{static_cast<int>(currentX), 20});

        auto ghost = SPRITES[SCARED_1 + flick];

        ghost.id = 12;
        API::Draw(SPRITES[TILE_NOTHING], Vec2i{ghostX + 3, 20});
        API::Draw(ghost, Vec2i{ghostX + 4, 20});
        ghost.id = 13;
        API::Draw(SPRITES[TILE_NOTHING], Vec2i{ghostX + 5, 20});
        API::Draw(ghost, Vec2i{ghostX + 6, 20});

        lastGhostX = ghostX;
    }

    if (mTimer > 21.0f && mTimer < 23.0f) {
        API::Draw(SPRITES[SCORE_800], Vec2i{lastGhostX + 4, 20});

        auto ghost = SPRITES[SCARED_1 + flick];

        ghost.id = 13;
        API::Draw(SPRITES[TILE_NOTHING], Vec2i{lastGhostX + 5, 20});
        API::Draw(ghost, Vec2i{lastGhostX + 6, 20});
    }

    if (mTimer > 23.0f && mTimer < 23.5f) {
        float currentX = Lerp(4.f, 30.f, 3.5f, mTimer - 21.5f);
        int ghostX = static_cast<int>(Lerp(6.f, 33.f, 7.f, mTimer - 21.5f));

        API::Draw(SPRITES[TILE_EMPTY], Vec2i{static_cast<int>(currentX) - 1, 20});
        API::Draw(PACMAN_XY(0, flick * 2),
            Vec2i{static_cast<int>(currentX), 20});

        auto ghost = SPRITES[SCARED_1 + flick];

        ghost.id = 13;
        API::Draw(SPRITES[TILE_NOTHING], Vec2i{ghostX + 5, 20});
        API::Draw(ghost, Vec2i{ghostX + 6, 20});

        lastGhostX = ghostX;
    }

    if (mTimer > 23.5f && mTimer < 25.5f) {
        API::Draw(SPRITES[SCORE_1600], Vec2i{lastGhostX + 6, 20});
    }

    if (mTimer > 25.5f && mTimer < 30.0f) {
        float currentX = Lerp(4.f, 30.f, 3.5f, mTimer - 23.5f);

        API::Draw(SPRITES[TILE_EMPTY], Vec2i{static_cast<int>(currentX) - 1, 20});
        API::Draw(PACMAN_XY(0, flick * 2),
            Vec2i{static_cast<int>(currentX), 20});
    }

    if (mTimer > 30.f) {
        mTimer = 0.f;
    }
}

///////////////////////////////////////////////////////////////////////////////
void Menu::DrawMenu(void)
{
    DrawMenuTextStatic();
    DrawMenuTextAnimated();
}

///////////////////////////////////////////////////////////////////////////////
void Menu::Tick(float deltaSeconds)
{
    mTimer += deltaSeconds;

    switch (mState) {
        case State::ROM_CHECKSUM:
            DrawRomChecksum();
            break;
        case State::MENU:
            DrawMenu();
            break;
    }
}

///////////////////////////////////////////////////////////////////////////////
void Menu::EndPlay(void)
{}

} // namespace Arc::Pacman
