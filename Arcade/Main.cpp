///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Arcade/core/Core.hpp"
#include "Arcade/errors/DLError.hpp"
#include <iostream>

///////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <library>" << std::endl;
        return (84);
    }

    try {
        Arc::Core core(argv[1], "lib/arcade_gui_menu.so");
        core.Run();
    } catch (const Arc::DLError& error) {
        std::cerr << error.what() << std::endl;
        return (84);
    }

    return (0);
}
