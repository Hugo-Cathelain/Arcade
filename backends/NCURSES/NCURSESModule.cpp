///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "backends/NCURSES/NCURSESModule.hpp"
#include "Arcade/core/API.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Arc
///////////////////////////////////////////////////////////////////////////////
namespace Arc
{

///////////////////////////////////////////////////////////////////////////////
NCURSESModule::NCURSESModule(void)
    : mWindow(nullptr)
{
    initscr();
    start_color();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    nodelay(stdscr, TRUE);
    timeout(0);

    int maxY, maxX;
    getmaxyx(stdscr, maxY, maxX);

    int windowHeight = 33;
    int windowWidth = 29 * 2;
    int startY = (maxY - windowHeight - 2) / 2;
    int startX = (maxX - windowWidth - 2) / 2;

    // Ensure window position is not negative
    startY = (startY < 0) ? 0 : startY;
    startX = (startX < 0) ? 0 : startX;

    mWindow = subwin(stdscr, windowHeight, windowWidth, startY, startX);

    if (mWindow == nullptr) {
        endwin();
    }
}

///////////////////////////////////////////////////////////////////////////////
NCURSESModule::~NCURSESModule()
{
    curs_set(1);
    delwin(mWindow);
    endwin();
    mWindow = nullptr;
}

///////////////////////////////////////////////////////////////////////////////
void NCURSESModule::Update(void)
{
    while (auto event = API::PollEvent(API::Event::GRAPHICS)) {
        if (auto gridSize = event->GetIf<API::Event::GridSize>()) {
            // Delete the old window
            if (mWindow != nullptr) {
                delwin(mWindow);
                mWindow = nullptr;
            }

            // Calculate new window dimensions
            int windowHeight = gridSize->height + 2;
            int windowWidth = gridSize->width * 2 + 2;

            // Calculate centered position
            int maxY, maxX;
            getmaxyx(stdscr, maxY, maxX);
            int startY = (maxY - windowHeight) / 2;
            int startX = (maxX - windowWidth) / 2;

            // Ensure window position is not negative
            startY = (startY < 0) ? 0 : startY;
            startX = (startX < 0) ? 0 : startX;

            // Create the new window
            mWindow = subwin(stdscr, windowHeight, windowWidth, startY, startX);

            // Make sure window was created successfully
            if (mWindow == nullptr) {
                // Handle error (could throw an exception or log an error)
            }
        }
    }

    int delay = nodelay(mWindow, TRUE);
    int ch = getch();
    if (ch != ERR) {
        switch (ch) {
            case KEY_UP:
                API::PushEvent(API::Event::Channel::GAME, API::Event::KeyPressed{EKeyboardKey::UP});
                break;
            case KEY_DOWN:
                API::PushEvent(API::Event::Channel::GAME, API::Event::KeyPressed{EKeyboardKey::DOWN});
                break;
            case KEY_LEFT:
                API::PushEvent(API::Event::Channel::GAME, API::Event::KeyPressed{EKeyboardKey::LEFT});
                break;
            case KEY_RIGHT:
                API::PushEvent(API::Event::Channel::GAME, API::Event::KeyPressed{EKeyboardKey::RIGHT});
                break;
            case ' ':
                API::PushEvent(API::Event::Channel::GAME, API::Event::KeyPressed{EKeyboardKey::SPACE});
                break;
            case '\n':
                API::PushEvent(API::Event::Channel::GAME, API::Event::KeyPressed{EKeyboardKey::ENTER});
                break;
            case 27: // ESC
                API::PushEvent(API::Event::Channel::CORE, API::Event::Closed());
                break;
            default:
                if (ch >= 'a' && ch <= 'z')
                    API::PushEvent(API::Event::Channel::GAME, API::Event::KeyPressed{static_cast<EKeyboardKey>('A' + (ch - 'a'))});
                else if (ch >= 'A' && ch <= 'Z')
                    API::PushEvent(API::Event::Channel::GAME, API::Event::KeyPressed{static_cast<EKeyboardKey>(ch)});
                else if (ch >= '0' && ch <= '9')
                    API::PushEvent(API::Event::Channel::GAME, API::Event::KeyPressed{static_cast<EKeyboardKey>('0' + (ch - '0'))});
                break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void NCURSESModule::Clear(void)
{
}

///////////////////////////////////////////////////////////////////////////////
void NCURSESModule::Render(void)
{
    while (!API::IsDrawQueueEmpty()) {
        auto draw = API::PopDraw();
        auto [asset, x, y] = draw;
        mvwprintw(mWindow, y + 1, (x * 2) + 1, asset.characters.c_str());
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
