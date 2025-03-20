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
