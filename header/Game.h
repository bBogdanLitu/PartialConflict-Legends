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
    unsigned int ans1 = 0, ans2 = 0;
    unsigned long currentTurn = 0;
    long int sarmale = 0; //this is the 'gold' of my game

    void PopulateGenerals(std::ifstream generalsJson);

    void PopulateSettlements(std::ifstream settlementsJson);

    void PopulateControlPoints(std::ifstream controlPointsJson);

    void PopulateCaptains(std::ifstream captainsJson);

    void CheckGenerals() const;

    void DisplayStartingGenerals() const;

    void ResetArmiesActionPoints() const;

    void CollectIncomeFromSettlements();

    void TickAttackCountdown();

    void NextTurn();



public:
    int Start();
};

#endif //Game_H
