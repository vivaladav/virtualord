#include "Game.h"

#ifdef DEV_MODE
#include <iostream>
#endif

int main(int argc, char * argv[])
{
#ifdef DEV_MODE
    std::cout << "=============================== DEVELOPER MODE ===============================" << std::endl;
#endif

    game::Game game(argc, argv);

    game.Run();

#ifdef DEV_MODE
    std::cout << "=============================== DEVELOPER MODE ===============================" << std::endl;
#endif

    return 0;
}
