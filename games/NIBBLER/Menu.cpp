///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "games/NIBBLER/Menu.hpp"
#include "games/NIBBLER/Assets.hpp"
#include "Arcade/core/API.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Arc::Nibbler
///////////////////////////////////////////////////////////////////////////////
namespace Arc::Nibbler
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
void Menu::BeginPlay(void)
{}

///////////////////////////////////////////////////////////////////////////////
void Menu::Text(
    const std::string& text,
    Menu::TextColor color,
    const Vec2i& position
)
{
    static const int LTRS_OFFSET_Y = 13;
    static const Color LTS_COLORS[] = {
        Color{255, 255, 222}, Color{0, 184, 151}, Color{255, 0, 0},
        Color{255, 255, 0}, Color{204, 29, 243}, Color{5, 12, 196},
        Color{168, 168, 139}
    };

    for (size_t i = 0; i < text.size(); i++) {
        int row = 0;
        int col = 0;

        if (text[i] >= 'A' && text[i] <= 'Z') {
            col = static_cast<int>(text[i] - 'A');
        } else if (text[i] >= '0' && text[i] <= '9') {
            col = static_cast<int>(text[i] - '0');
            row = 1;
        } else if (text[i] == ',') {
            col = 10;
            row = 1;
        } else if (text[i] == '@') {
            col = 26;
            row = 0;
        } else {
            continue;
        }

        IGameModule::Asset sprite(
            {col, LTRS_OFFSET_Y + row + static_cast<int>(color) * 2},
            std::string(1, text[i]),
            LTS_COLORS[static_cast<int>(color)]
        );

        API::Draw(sprite, position + Vec2i{static_cast<int>(i), 0});
    }
}

///////////////////////////////////////////////////////////////////////////////
void Menu::DrawGameInformation(void)
{
    API::Draw(SPRITES[TEXT_PLAYER], Vec2i(1, 0));
    API::Draw(SPRITES[TEXT_1], Vec2i(3, 0));
    Text("0", TextColor::TEXT_WHITE, {15, 0});

    API::Draw(SPRITES[TEXT_LEFT], Vec2i(22, 0));
    Text("0", TextColor::TEXT_WHITE, {26, 0});

    API::Draw(SPRITES[TEXT_PLAYER], Vec2i(1, 1));
    API::Draw(SPRITES[TEXT_2], Vec2i(3, 1));
    Text("0", TextColor::TEXT_WHITE, {15, 1});

    API::Draw(SPRITES[TEXT_LEFT], Vec2i(22, 1));
    Text("0", TextColor::TEXT_WHITE, {26, 1});

    API::Draw(SPRITES[TEXT_HISCORE], Vec2i(2, 3));
    Text("50,000", TextColor::TEXT_CYAN, {10, 3});

    Text("TIME", TextColor::TEXT_YELLOW, {19, 3});
    Text("0", TextColor::TEXT_WHITE, {26, 3});

    Text("C", TextColor::TEXT_WHITE, {27, 12});
    Text("R", TextColor::TEXT_WHITE, {27, 13});
    Text("E", TextColor::TEXT_WHITE, {27, 14});
    Text("D", TextColor::TEXT_WHITE, {27, 15});
    Text("I", TextColor::TEXT_WHITE, {27, 16});
    Text("T", TextColor::TEXT_WHITE, {27, 17});
    Text("0", TextColor::TEXT_WHITE, {27, 19});

    Text("WAVE   0", TextColor::TEXT_WHITE, {10, 31});
}

///////////////////////////////////////////////////////////////////////////////
void Menu::DrawROMChecksum(void)
{
    Text("POWER UP DIAGNOSTICS", TextColor::TEXT_PINK, {4, 3});
    Text("RAM", TextColor::TEXT_CYAN, {13, 5});

    Text("IC 13", TextColor::TEXT_PINK, {2, 7});
    Text("IC 32", TextColor::TEXT_PINK, {2, 9});
    Text("IC 33", TextColor::TEXT_PINK, {2, 11});
    Text("IC 34", TextColor::TEXT_PINK, {2, 13});
    Text("IC 35", TextColor::TEXT_PINK, {2, 15});

    Text("IC 36", TextColor::TEXT_PINK, {16, 7});
    Text("IC 37", TextColor::TEXT_PINK, {16, 9});
    Text("IC 67", TextColor::TEXT_PINK, {16, 11});
    Text("IC 68", TextColor::TEXT_PINK, {16, 13});

    if (mTimer > .0f) Text("OK", TextColor::TEXT_YELLOW, {9, 7});
    if (mTimer > .2f) Text("OK", TextColor::TEXT_YELLOW, {9, 9});
    if (mTimer > .3f) Text("OK", TextColor::TEXT_YELLOW, {9, 11});
    if (mTimer > .5f) Text("OK", TextColor::TEXT_YELLOW, {9, 13});
    if (mTimer > .6f) Text("OK", TextColor::TEXT_YELLOW, {9, 15});

    if (mTimer > .8f) Text("OK", TextColor::TEXT_YELLOW, {23, 7});
    if (mTimer > .85f) Text("OK", TextColor::TEXT_YELLOW, {23, 9});
    if (mTimer > .90f) Text("OK", TextColor::TEXT_YELLOW, {23, 11});
    if (mTimer > .95f) Text("OK", TextColor::TEXT_YELLOW, {23, 13});

    Text("PROM", TextColor::TEXT_CYAN, {12, 18});

    Text("IC  7", TextColor::TEXT_PINK, {2, 22});
    Text("IC  8", TextColor::TEXT_PINK, {2, 24});
    Text("IC  9", TextColor::TEXT_PINK, {2, 26});
    Text("IC 10", TextColor::TEXT_PINK, {2, 28});

    Text("IC 12", TextColor::TEXT_PINK, {16, 22});
    Text("IC 15", TextColor::TEXT_PINK, {16, 24});
    Text("IC 16", TextColor::TEXT_PINK, {16, 26});
    Text("IC 17", TextColor::TEXT_PINK, {16, 28});

    if (mTimer > 1.1f) Text("OK", TextColor::TEXT_YELLOW, {9, 22});
    if (mTimer > 1.2f) Text("OK", TextColor::TEXT_YELLOW, {9, 24});
    if (mTimer > 1.3f) Text("OK", TextColor::TEXT_YELLOW, {9, 26});
    if (mTimer > 1.5f) Text("OK", TextColor::TEXT_YELLOW, {9, 28});

    if (mTimer > 1.6f) Text("OK", TextColor::TEXT_YELLOW, {23, 22});
    if (mTimer > 1.8f) Text("OK", TextColor::TEXT_YELLOW, {23, 24});
    if (mTimer > 2.0f) Text("OK", TextColor::TEXT_YELLOW, {23, 26});
    if (mTimer > 2.1f) Text("OK", TextColor::TEXT_YELLOW, {23, 28});

    if (mTimer > 3.f) {
        mState = State::TITLESCREEN;
        mTimer = 0.f;
    }
}

///////////////////////////////////////////////////////////////////////////////
void Menu::DrawTitleScreen(void)
{
    DrawGameInformation();

    API::Draw(SPRITES[GAME_NAME], Vec2i(14, 7));
    API::Draw(SPRITES[GAME_MASCOT], Vec2i(14, 18));

    int progress = mTimer / 1.5f * 26;

    for (int i = progress; i < 26; i++) {
        for (int y = 0; y < 22; y++) {
            API::Draw(SPRITES[TRON_SQUARE], Vec2i(1 + i, 4 + y));
        }
    }

    if (mTimer > 2.f) {
        Text("@ 1982 ROCK OLA MFG CORP", TextColor::TEXT_PINK, {3, 26});
    }
}

///////////////////////////////////////////////////////////////////////////////
void Menu::DrawInstruction(void)
{
    DrawGameInformation();

    static const float CHARS_PER_SECOND = 20.f;
    static const std::vector<std::pair<TextColor, std::string>> instructions =
    {
        {TextColor::TEXT_WHITE, "INSTRUCTIONS"},
        {TextColor::TEXT_YELLOW, "CLEAR EACH MAZE BEFORE"},
        {TextColor::TEXT_YELLOW, "TIME RUNS OUT"},
        {TextColor::TEXT_RED, "DO NOT LET NIBBLER"},
        {TextColor::TEXT_RED, "BITE ITSELF"},
        {TextColor::TEXT_PINK, "NIBBLER DOES NOT STOP"},
        {TextColor::TEXT_PINK, "AT CORNERS"},
        {TextColor::TEXT_CYAN, "NIBBLER GROWS LONGER"},
        {TextColor::TEXT_CYAN, "WHEN IT EATS"},
        {TextColor::TEXT_RED, "TIME DECREASES FASTER"},
        {TextColor::TEXT_RED, "WHEN NIBBLER DOES NOT EAT"},
        {TextColor::TEXT_PINK, "BONUS IS TIME LEFT AT"},
        {TextColor::TEXT_PINK, "END OF WAVE"},
        {TextColor::TEXT_YELLOW, "EXTRA NIBBLER EVERY 4 WAVES"}
    };

    Vec2i position(0, 7);
    TextColor lastColor = TextColor::TEXT_WHITE;
    int totalVisibleChars = static_cast<int>(mTimer * CHARS_PER_SECOND);
    int charsProcessed = 0;

    for (const auto& [color, text] : instructions) {
        if (lastColor != color) {
            position.y++;
            lastColor = color;
        }

        position.x = (28 - text.size()) / 2;

        if (charsProcessed >= totalVisibleChars) {
            break;
        }

        std::string displayText;
        if (
            charsProcessed + text.size() <=
            static_cast<unsigned int>(totalVisibleChars)
        ) {
            displayText = text;
        } else {
            int charsToShowInThisLine = totalVisibleChars - charsProcessed;
            displayText = text.substr(0, charsToShowInThisLine);
        }

        Text(displayText, color, position);

        charsProcessed += text.size();
        position.y++;
    }
}

///////////////////////////////////////////////////////////////////////////////
void Menu::Tick(float deltaSeconds)
{
    mTimer += deltaSeconds;

    switch (mState)
    {
        case State::ROM_CHECKSUM:
            DrawROMChecksum();
            break;
        case State::TITLESCREEN:
            DrawTitleScreen();
            break;
        case State::INSTRUCTION:
            DrawInstruction();
            break;
    }
}

///////////////////////////////////////////////////////////////////////////////
void Menu::EndPlay(void)
{}

///////////////////////////////////////////////////////////////////////////////
bool Menu::ToggleInstruction(void)
{
    if (mState != State::INSTRUCTION) {
        mState = State::INSTRUCTION;
        mTimer = 0.f;
        return (true);
    }
    return (false);
}

} // namespace Arc::Nibbler
