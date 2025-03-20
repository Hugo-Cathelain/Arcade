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
    , mIsWindowOpen(true)
{
    mStates.push(Library::Load<IGameModule>(gameLib));
    mGraphics->LoadSpriteSheet(mStates.top()->GetSpriteSheet());
}

///////////////////////////////////////////////////////////////////////////////
void Core::HandleEvents(void)
{
    while (std::optional event = API::PollEvent(API::Event::CORE)) {
        if (auto change = event->GetIf<API::Event::ChangeGraphics>()) {
            // TODO: Change the graphic library (load sprite sheet again)
        }
        if (auto change = event->GetIf<API::Event::ChangeGame>()) {
            // TODO: Change the game libary (endplay and beginplay)
        }
        if (event->Is<API::Event::Closed>()) {
            mIsWindowOpen = false;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void Core::Run(void)
{
    if (mStates.size() == 0) {
        return;
    }

    mGraphics->SetTitle(mStates.top()->GetName());

    mStates.top()->BeginPlay();
    while (mIsWindowOpen && mStates.size() > 0) {
        HandleEvents();
        mGraphics->Update();
        mGraphics->Clear();
        mStates.top()->Tick(0.f);
        mGraphics->Render();
    }
    mStates.top()->EndPlay();
}

} // namespace Arc
