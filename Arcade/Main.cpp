///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Arcade/core/Core.hpp"
#include "Arcade/errors/DLError.hpp"
#include <iostream>

///////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " <library> <game>" << std::endl;
        return (84);
    }

    try {
        Arc::Core core(argv[1], argv[2]);
        core.Run();
    } catch (const Arc::DLError& error) {
        std::cerr << error.what() << std::endl;
        return (84);
    }

    return (0);
}
