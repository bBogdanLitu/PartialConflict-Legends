#ifndef Game_H
#define Game_H
#include <iostream>
#include <vector>
#include "Settlement.h"

class Game {
private:
    std::vector<std::shared_ptr<Unit> > StartingGenerals, PlayerGenerals, ContenderGenerals, WarlordGenerals,
            EmperorGenerals, Captains;
    std::vector<Settlement> Settlements;
    unsigned int ans1 = 0, ans2 = 0, ans3 = 0;

    void PopulateGenerals(std::ifstream generalsJson);

    void PopulateSettlements(std::ifstream settlementsJson);

    void PopulateControlPoints(std::ifstream controlPointsJson);

    void PopulateCaptains(std::ifstream captainsJson);

    void CheckGenerals() const;

    void DisplayStartingGenerals() const;

public:
    int Start();
};

#endif //Game_H
