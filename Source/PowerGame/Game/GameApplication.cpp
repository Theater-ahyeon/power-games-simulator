#include "ConsoleGame.h"

#if defined(USE_GUI) && defined(SDL2_SUPPORT)
#include "GUIGame.h"
#endif

int main(int argc, char* argv[]) {
#if defined(USE_GUI) && defined(SDL2_SUPPORT)
    return PowerGame::GUIGame::Instance().Run();
#else
    return PowerGame::ConsoleGame::Instance().Run();
#endif
}
