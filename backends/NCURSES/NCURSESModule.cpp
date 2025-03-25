///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "backends/NCURSES/NCURSESModule.hpp"
#include "Arcade/core/API.hpp"
#include <iostream>

///////////////////////////////////////////////////////////////////////////////
// Namespace Arc
///////////////////////////////////////////////////////////////////////////////
namespace Arc
{

///////////////////////////////////////////////////////////////////////////////
void NCURSESModule::InitColor(void)
{
    start_color();
    use_default_colors();
}

///////////////////////////////////////////////////////////////////////////////
int NCURSESModule::FindOrCreateColor(short r, short g, short b)
{
    short scaledR = (r * 1000) / 255;
    short scaledG = (g * 1000) / 255;
    short scaledB = (b * 1000) / 255;

    std::tuple<short, short, short> colorKey = std::make_tuple(scaledR, scaledG, scaledB);

    auto it = mColorMap.find(colorKey);
    if (it != mColorMap.end()) {
        return (it->second);
    }

    if (mColorPairCount < COLOR_PAIRS) {
        init_color(mColorPairCount, scaledR, scaledG, scaledB);
        init_pair(mColorPairCount, mColorPairCount, -1);
        mColorMap[colorKey] = mColorPairCount;
        return (mColorPairCount++);
    }

    return (0);
}

///////////////////////////////////////////////////////////////////////////////
NCURSESModule::NCURSESModule(void)
    : mWindow(nullptr)
    , mColorPairCount(1)
{
    initscr();
    refresh();
    start_color();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    nodelay(stdscr, TRUE);
    timeout(0);

    int maxY, maxX;
    getmaxyx(stdscr, maxY, maxX);

    if (maxY < 35 || maxX < 60) {
        endwin();
        std::cerr << "Terminal window is too small." << std::endl;
        API::PushEvent(API::Event::CORE, API::Event::Closed{});
        return;
    }

    int windowHeight = 33;
    int windowWidth = 29 * 2;
    int startY = (maxY - windowHeight - 2) / 2;
    int startX = (maxX - windowWidth - 2) / 2;

    startY = (startY < 0) ? 0 : startY;
    startX = (startX < 0) ? 0 : startX;

    mWindow = subwin(stdscr, windowHeight, windowWidth, startY, startX);

    if (mWindow == nullptr) {
        endwin();
        API::PushEvent(API::Event::CORE, API::Event::Closed{});
        return;
    }

    InitColor();
}

///////////////////////////////////////////////////////////////////////////////
NCURSESModule::~NCURSESModule()
{
    mColorMap.clear();
    if (mWindow != nullptr) {
        delwin(mWindow);
        mWindow = nullptr;
    }
    endwin();
}

///////////////////////////////////////////////////////////////////////////////
EKeyboardKey NCURSESModule::GetKey(int key)
{
    switch (key) {
        case 'a': case 'A':         return (EKeyboardKey::A);
        case 'b': case 'B':         return (EKeyboardKey::B);
        case 'c': case 'C':         return (EKeyboardKey::C);
        case 'd': case 'D':         return (EKeyboardKey::D);
        case 'e': case 'E':         return (EKeyboardKey::E);
        case 'f': case 'F':         return (EKeyboardKey::F);
        case 'g': case 'G':         return (EKeyboardKey::G);
        case 'h': case 'H':         return (EKeyboardKey::H);
        case 'i': case 'I':         return (EKeyboardKey::I);
        case 'j': case 'J':         return (EKeyboardKey::J);
        case 'k': case 'K':         return (EKeyboardKey::K);
        case 'l': case 'L':         return (EKeyboardKey::L);
        case 'm': case 'M':         return (EKeyboardKey::M);
        case 'n': case 'N':         return (EKeyboardKey::N);
        case 'o': case 'O':         return (EKeyboardKey::O);
        case 'p': case 'P':         return (EKeyboardKey::P);
        case 'q': case 'Q':         return (EKeyboardKey::Q);
        case 'r': case 'R':         return (EKeyboardKey::R);
        case 's': case 'S':         return (EKeyboardKey::S);
        case 't': case 'T':         return (EKeyboardKey::T);
        case 'u': case 'U':         return (EKeyboardKey::U);
        case 'v': case 'V':         return (EKeyboardKey::V);
        case 'w': case 'W':         return (EKeyboardKey::W);
        case 'x': case 'X':         return (EKeyboardKey::X);
        case 'y': case 'Y':         return (EKeyboardKey::Y);
        case 'z': case 'Z':         return (EKeyboardKey::Z);
        case KEY_UP:                return (EKeyboardKey::UP);
        case KEY_DOWN:              return (EKeyboardKey::DOWN);
        case KEY_LEFT:              return (EKeyboardKey::LEFT);
        case KEY_RIGHT:             return (EKeyboardKey::RIGHT);
        case '0':                   return (EKeyboardKey::NUM0);
        case '1':                   return (EKeyboardKey::NUM1);
        case '2':                   return (EKeyboardKey::NUM2);
        case '3':                   return (EKeyboardKey::NUM3);
        case '4':                   return (EKeyboardKey::NUM4);
        case '5':                   return (EKeyboardKey::NUM5);
        case '6':                   return (EKeyboardKey::NUM6);
        case '7':                   return (EKeyboardKey::NUM7);
        case '8':                   return (EKeyboardKey::NUM8);
        case '9':                   return (EKeyboardKey::NUM9);
        case ' ':                   return (EKeyboardKey::SPACE);
        case '\n': case KEY_ENTER:  return (EKeyboardKey::ENTER);
        case 27:                    return (EKeyboardKey::ESCAPE);
        case KEY_BACKSPACE:         return (EKeyboardKey::BACKSPACE);
        default:                    return (EKeyboardKey::UNKNOWN);
    }
}

///////////////////////////////////////////////////////////////////////////////
void NCURSESModule::ResetNCURSES(void)
{
    if (mWindow != nullptr) {
        delwin(mWindow);
        mWindow = nullptr;
    }

    mColorPairCount = 1;
    mColorMap.clear();

    endwin();
    initscr();
    refresh();
    start_color();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    nodelay(stdscr, TRUE);
    timeout(0);

    int maxY, maxX;
    getmaxyx(stdscr, maxY, maxX);

    if (maxY < 35 || maxX < 60) {
        endwin();
        std::cerr << "Terminal window is too small." << std::endl;
        API::PushEvent(API::Event::CORE, API::Event::Closed{});
        return;
    }

    int windowHeight = 33;
    int windowWidth = 29 * 2;
    int startY = (maxY - windowHeight - 2) / 2;
    int startX = (maxX - windowWidth - 2) / 2;

    startY = (startY < 0) ? 0 : startY;
    startX = (startX < 0) ? 0 : startX;

    mWindow = subwin(stdscr, windowHeight, windowWidth, startY, startX);

    if (mWindow == nullptr) {
        endwin();
        API::PushEvent(API::Event::CORE, API::Event::Closed{});
        return;
    }

    clear();
    wclear(mWindow);

    InitColor();
}

///////////////////////////////////////////////////////////////////////////////
void NCURSESModule::Update(void)
{
    while (auto event = API::PollEvent(API::Event::GRAPHICS)) {
        if (auto gridSize = event->GetIf<API::Event::GridSize>()) {
            if (mWindow != nullptr) {
                delwin(mWindow);
                mWindow = nullptr;
            }

            int windowHeight = gridSize->height + 2;
            int windowWidth = gridSize->width * 2 + 2;

            int maxY, maxX;
            getmaxyx(stdscr, maxY, maxX);
            int startY = (maxY - windowHeight) / 2;
            int startX = (maxX - windowWidth) / 2;

            startY = (startY < 0) ? 0 : startY;
            startX = (startX < 0) ? 0 : startX;

            mWindow = subwin(stdscr, windowHeight, windowWidth, startY, startX);

            if (mWindow == nullptr) {
                endwin();
                return;
            }

            clear();
        } else if (event->Is<API::Event::ChangeGame>()) {
            ResetNCURSES();
        }
    }

    if (mWindow == nullptr) {
        return;
    }

    nodelay(mWindow, TRUE);
    int ch = getch();
    if (ch != ERR) {
        API::PushEvent(
            API::Event::CORE,
            API::Event::KeyPressed{GetKey(ch)}
        );
    }
}

///////////////////////////////////////////////////////////////////////////////
void NCURSESModule::Clear(void)
{}

///////////////////////////////////////////////////////////////////////////////
void NCURSESModule::Render(void)
{
    if (mWindow == nullptr) {
        return;
    }

    while (!API::IsDrawQueueEmpty()) {
        auto draw = API::PopDraw();
        auto [asset, pos, color] = draw;
        int y = pos.y, x = pos.x;

        int colorPair = FindOrCreateColor(asset.color.r, asset.color.g, asset.color.b);

        wattron(mWindow, COLOR_PAIR(colorPair));
        wattroff(mWindow, A_DIM);
        mvwprintw(mWindow, y + 1, (x * 2) + 1, asset.characters.c_str());
        wattroff(mWindow, COLOR_PAIR(colorPair));
    }
    box(mWindow, ACS_VLINE, ACS_HLINE);
    refresh();
    wrefresh(mWindow);
}

///////////////////////////////////////////////////////////////////////////////
void NCURSESModule::SetTitle(const std::string& title)
{
    (void)title;
}

///////////////////////////////////////////////////////////////////////////////
void NCURSESModule::LoadSpriteSheet(const std::string& path)
{
    (void)path;
}

} // namespace Arc
