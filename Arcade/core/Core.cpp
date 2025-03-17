///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Arcade/core/Core.hpp"
#include "Arcade/core/Library.hpp"
#include "Arcade/core/API.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Arc
///////////////////////////////////////////////////////////////////////////////
namespace Arc
{

///////////////////////////////////////////////////////////////////////////////
Core::Core(const std::string& graphicLib, const std::string& gameLib)
    : mGraphics(Library::Load<IGraphicsModule>(graphicLib))
    , mGame(Library::Load<IGameModule>(gameLib))
{}

///////////////////////////////////////////////////////////////////////////////
void Core::HandleEvents(void)
{
    while (std::optional event = API::PollEvent(API::Event::CORE)) {
        if (auto change = event->GetIf<API::Event::ChangeGraphics>()) {
            // TODO: Change the graphic library
        }
        if (auto change = event->GetIf<API::Event::ChangeGame>()) {
            // TODO: Change the game libary (endplay and beginplay)
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void Core::Run(void)
{
    mGraphics->SetTitle(mGame->GetName());

    mGame->BeginPlay();
    while (!mGame->IsGameOver()) {
        HandleEvents();
        mGraphics->Update();
        mGraphics->Clear();
        mGame->Tick(0.f);
        mGraphics->Render();
    }
    mGame->EndPlay();
}

} // namespace Arc
