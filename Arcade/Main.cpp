///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Arcade/core/Core.hpp"
#include "Arcade/errors/DLError.hpp"
#include <iostream>

///////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
    if (argc < 2 || argc > 3) {
        std::cout << "Usage: " << argv[0] << " <library> [game]" << std::endl;
        return (84);
    }

    std::string game = "lib/arcade_gui_menu.so";

    if (argc == 3) {
        game = argv[2];
    }

    try {
        Arc::Core core(argv[1], game);
        core.Run();
    } catch (const Arc::Exception& error) {
        std::cerr << error.what() << std::endl;
        return (84);
    } catch (const std::exception& error) {
        std::cerr << "Unexpected error: " << error.what() << std::endl;
        return (84);
    }

    return (0);
}
