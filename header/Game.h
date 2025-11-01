#ifndef Game_H
#define Game_H
#include <iostream>
#include <vector>
#include "Settlement.h"

class Game {
private:
    std::vector<General> StartingGenerals, PlayerGenerals, ContenderGenerals, WarlordGenerals, EmperorGenerals;
    std::vector<Settlement> Settlements;
    unsigned int ans1 = 0, ans2 = 0;

    void PopulateGenerals(std::ifstream generalsJson);

    void PopulateSettlements(std::ifstream settlementsJson);

    void CheckGenerals() const;

    void DisplayStartingGenerals() const;

public:
    int Start();
};

#endif //Game_H
