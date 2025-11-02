#include "header/Game.h"
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/dom/table.hpp>
#include <iostream>

#include "header/Captain.h"
#include "header/Unit.h"

int main() {
    /*Game game;
    game.Start();*/

    //Initial tests for INHERITANCE
    Captain captain1 = {"John", "Pork", 0, 1, 50, 50, 50, 50, 50, 50, 0.7};
    Captain captain2 = {"Big, Bad", "Wolf", 0, 1, 60, 60, 60, 60, 60, 50, 0.7};

    captain1.FightWith(captain2, 0) ? std::cout << "John Pork escapes!\n" : std::cout << "John Pork is mauled to death...\n";

    std::shared_ptr<Unit> cap1 = captain1.clone();
    std::shared_ptr<Unit> cap2 = captain2.clone();

    cap1->FightWith(*cap2, 850) ? std::cout << "John Pork escapes!\n" : std::cout << "John Pork is mauled to death...\n";

    return 0;
}

