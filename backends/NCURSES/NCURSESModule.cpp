///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "backends/NCURSES/NCURSESModule.hpp"
#include "Arcade/core/API.hpp"
#include <iostream>
#include <cerrno>
#include <cstring>

///////////////////////////////////////////////////////////////////////////////
// Namespace Arc
///////////////////////////////////////////////////////////////////////////////
namespace Arc
{

///////////////////////////////////////////////////////////////////////////////
NCURSESModule::NCURSESModule(void)
    : mWindow(nullptr)
    , mHasColor(false)
    , mWidth(80)
    , mHeight(24)
{
    if (isendwin() == FALSE) {
        endwin();
    }

    initscr();

    if (isendwin()) {
        refresh();
    }

    if (has_colors() == TRUE) {
        start_color();
        use_default_colors();
        for (int i = 1; i < COLOR_PAIRS; ++i) {
            init_pair(i, -1, -1);
        }
    }

    int termWidth, termHeight;
    getmaxyx(stdscr, termHeight, termWidth);

    int startY = (termHeight - mHeight) / 2;
    int startX = (termWidth - mWidth) / 2;

    mWindow = newwin(mHeight, mWidth, startY, startX);
    if (mWindow == nullptr) {
        endwin();
        throw std::runtime_error("Failed to create NCURSES window");
    }

    keypad(mWindow, TRUE);
    nodelay(mWindow, TRUE);
    noecho();
    curs_set(0);

    mHasColor = (has_colors() == TRUE && can_change_color() == TRUE);
    if (mHasColor) {
        start_color();
        use_default_colors();
    }

    mousemask(ALL_MOUSE_EVENTS, nullptr);
}

///////////////////////////////////////////////////////////////////////////////
NCURSESModule::~NCURSESModule()
{
    if (mWindow) {
        delwin(mWindow);
        mWindow = nullptr;
    }
    endwin();
}

///////////////////////////////////////////////////////////////////////////////
EKeyboardKey NCURSESModule::MapNCursesKey(int key)
{
    switch (key) {
        // Alphabet keys
        case 'a': case 'A': return (EKeyboardKey::A);
        case 'b': case 'B': return (EKeyboardKey::B);
        case 'c': case 'C': return (EKeyboardKey::C);
        case 'd': case 'D': return (EKeyboardKey::D);
        case 'e': case 'E': return (EKeyboardKey::E);
        case 'f': case 'F': return (EKeyboardKey::F);
        case 'g': case 'G': return (EKeyboardKey::G);
        case 'h': case 'H': return (EKeyboardKey::H);
        case 'i': case 'I': return (EKeyboardKey::I);
        case 'j': case 'J': return (EKeyboardKey::J);
        case 'k': case 'K': return (EKeyboardKey::K);
        case 'l': case 'L': return (EKeyboardKey::L);
        case 'm': case 'M': return (EKeyboardKey::M);
        case 'n': case 'N': return (EKeyboardKey::N);
        case 'o': case 'O': return (EKeyboardKey::O);
        case 'p': case 'P': return (EKeyboardKey::P);
        case 'q': case 'Q': return (EKeyboardKey::Q);
        case 'r': case 'R': return (EKeyboardKey::R);
        case 's': case 'S': return (EKeyboardKey::S);
        case 't': case 'T': return (EKeyboardKey::T);
        case 'u': case 'U': return (EKeyboardKey::U);
        case 'v': case 'V': return (EKeyboardKey::V);
        case 'w': case 'W': return (EKeyboardKey::W);
        case 'x': case 'X': return (EKeyboardKey::X);
        case 'y': case 'Y': return (EKeyboardKey::Y);
        case 'z': case 'Z': return (EKeyboardKey::Z);

        // Special keys
        case KEY_UP:    return (EKeyboardKey::UP);
        case KEY_DOWN:  return (EKeyboardKey::DOWN);
        case KEY_LEFT:  return (EKeyboardKey::LEFT);
        case KEY_RIGHT: return (EKeyboardKey::RIGHT);
        case ' ':       return (EKeyboardKey::SPACE);
        case 10:        return (EKeyboardKey::ENTER);
        case 27:        return (EKeyboardKey::ESCAPE);
        case KEY_BACKSPACE: return (EKeyboardKey::BACKSPACE);

        // Number keys
        case '0': return (EKeyboardKey::NUM0);
        case '1': return (EKeyboardKey::NUM1);
        case '2': return (EKeyboardKey::NUM2);
        case '3': return (EKeyboardKey::NUM3);
        case '4': return (EKeyboardKey::NUM4);
        case '5': return (EKeyboardKey::NUM5);
        case '6': return (EKeyboardKey::NUM6);
        case '7': return (EKeyboardKey::NUM7);
        case '8': return (EKeyboardKey::NUM8);
        case '9': return (EKeyboardKey::NUM9);

        default: return (EKeyboardKey::UNKNOWN);
    }
}

///////////////////////////////////////////////////////////////////////////////
void NCURSESModule::Update(void)
{
    while (auto event = API::PollEvent(API::Event::GRAPHICS)) {
        if (auto gridSize = event->GetIf<API::Event::GridSize>()) {
            mWidth = (gridSize->width * 2) + 2;
            mHeight = (gridSize->height * 1) + 2;

            int termWidth, termHeight;
            getmaxyx(stdscr, termHeight, termWidth);

            int startY = (termHeight - mHeight) / 2;
            int startX = (termWidth - mWidth) / 2;

            clear();
            refresh();

            if (mWindow) {
                delwin(mWindow);
            }
            mWindow = newwin(mHeight, mWidth, startY, startX);

            keypad(mWindow, TRUE);
            nodelay(mWindow, TRUE);
            noecho();
            curs_set(0);

            box(mWindow, 0, 0);
        }
    }

    int ch = wgetch(mWindow);
    if (ch != ERR) {
        EKeyboardKey key = MapNCursesKey(ch);
        if (key != EKeyboardKey::UNKNOWN) {
            API::PushEvent(API::Event::CORE, API::Event::KeyPressed{key});
        }
    }

    MEVENT event;
    if (getmouse(&event) == OK) {
        EMouseButton button;
        switch (event.bstate) {
            case BUTTON1_PRESSED: button = EMouseButton::LEFT; break;
            case BUTTON2_PRESSED: button = EMouseButton::MIDDLE; break;
            case BUTTON3_PRESSED: button = EMouseButton::RIGHT; break;
            default: return;
        }

        int relativeX = event.x - getbegx(mWindow);
        int relativeY = event.y - getbegy(mWindow);

        API::PushEvent(API::Event::CORE, API::Event::MousePressed{
            button, relativeX, relativeY
        });
    }

    wrefresh(mWindow);
}

///////////////////////////////////////////////////////////////////////////////
void NCURSESModule::Clear(void)
{
    werase(mWindow);
}

///////////////////////////////////////////////////////////////////////////////
void NCURSESModule::Render(void)
{
    static int colorPairCounter = 1;
    std::map<std::tuple<short, short, short>, int> colorPairMap;

    colorPairMap.clear();
    colorPairCounter = 1;

    while (!API::IsDrawQueueEmpty()) {
        auto draw = API::PopDraw();
        auto [asset, pos, color] = draw;

        if (mHasColor) {
            short r = asset.color.r;
            short g = asset.color.g;
            short b = asset.color.b;

            auto colorKey = std::make_tuple(r, g, b);

            int colorPairIndex;
            auto it = colorPairMap.find(colorKey);
            if (it == colorPairMap.end()) {
                if (colorPairCounter >= COLOR_PAIRS) {
                    colorPairCounter = 1;
                }

                short ncursesR = (r * 1000) / 255;
                short ncursesG = (g * 1000) / 255;
                short ncursesB = (b * 1000) / 255;

                init_color(colorPairCounter, ncursesR, ncursesG, ncursesB);
                init_pair(colorPairCounter, colorPairCounter, -1);

                colorPairMap[colorKey] = colorPairCounter;
                colorPairIndex = colorPairCounter;

                colorPairCounter++;
            } else {
                colorPairIndex = it->second;
            }

            wattron(mWindow, COLOR_PAIR(colorPairIndex));
            mvwprintw(mWindow,
                pos.y + 1, (pos.x * 2) + 1,
                asset.characters.c_str()
            );
            wattroff(mWindow, COLOR_PAIR(colorPairIndex));
        } else {
            mvwprintw(mWindow,
                pos.y + 1, (pos.x * 2) + 1,
                asset.characters.c_str()
            );
        }
    }

    box(mWindow, 0, 0);
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
