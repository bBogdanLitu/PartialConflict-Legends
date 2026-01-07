#include "header/Game.h"
#include "header/Except.h"

int main() {
    try {
        Game game;
        game.Start();
    } catch (const ObjectFail &err) {
        std::cerr << "Object creation fail - " << err.what() << std::endl;
    } catch (const BrokenFile &err) {
        std::cerr << "Broken file - " << err.what() << std::endl;
    } catch (const ApplicationException &err) {
        std::cerr << "Application exception - " << err.what() << std::endl;
    }


    return 0;
}

