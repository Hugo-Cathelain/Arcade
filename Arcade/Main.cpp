///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Arcade/core/Library.hpp"
#include "Arcade/interfaces/IGraphicsModule.hpp"
#include "Arcade/interfaces/IGameModule.hpp"

///////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " <library> <game>" << std::endl;
        return (84);
    }

    auto graphics = Arc::Library::Load<Arc::IGraphicsModule>(argv[1]);
    auto game     = Arc::Library::Load<Arc::IGameModule>(argv[2]);

    if (!graphics || !game) {
        return (84);
    }

    graphics->SetTitle(game->GetName());

    game->BeginPlay();
    while (!game->IsGameOver()) {
        graphics->Update();
        graphics->Clear();
        game->Tick(0.f);
        graphics->Render();
    }
    game->EndPlay();

    return (0);
}
