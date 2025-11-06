#include <cstring>

#include "header/Game.h"
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/dom/table.hpp>
#include <iostream>
#include <termios.h>

#include "header/Captain.h"
#include "header/Unit.h"

int main() {
    static struct FixTermios {
        FixTermios() {
            struct termios t;
            if (tcgetattr(0, &t) == 0) {
                memset(&t, 0, sizeof(t));
                tcgetattr(0, &t);  // Re-fill with valid data
            }
        }
    } fix_termios;
    Game game;
    game.Start();

    return 0;
}

