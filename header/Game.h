#ifndef Game_H
#define Game_H
#include <iostream>
#include <vector>
#include <ftxui/component/loop.hpp>
#include "Settlement.h"
#include "Enemy.h"

class Game {
private:
    std::vector<std::shared_ptr<Unit> > StartingGenerals, PlayerGenerals, ContenderGenerals, WarlordGenerals,
            EmperorGenerals, Captains;
    std::vector<std::shared_ptr<Settlement> > Settlements;
    std::vector<std::shared_ptr<Enemy> > Enemies;
    unsigned int ans1 = 0, ans2 = 0;
    unsigned long currentTurn = 0;
    long int sarmale = 0; //this is the 'gold' of my game

    void PopulateEnemies(std::ifstream enemiesJson);

    void PopulateGenerals(std::ifstream generalsJson);

    void PopulateSettlements(std::ifstream settlementsJson);

    void PopulateControlPoints(std::ifstream controlPointsJson);

    void PopulateCaptains(std::ifstream captainsJson);

    void CheckGenerals() const;

    [[nodiscard]] ftxui::Table CreateStartingGeneralsTable() const;

    void DisplayStartingGenerals() const;

    [[nodiscard]] ftxui::Element FTXUIDisplayStaringGenerals() const;

    static void FTXUIDisplaySettlementAndArmy(const ftxui::Component &whereToDisplay, const Settlement &settlement);

    void ResetArmiesActionPoints() const;

    void CollectIncomeFromSettlements();

    //void TickAttackCountdown();

    void NextTurn();

public:
    int Start();

    //useful to add text, tables and other non-interactive elements
    static void AddElementToFTXUIContainer(const ftxui::Component &gameFlowWindow, const ftxui::Element &thingToAdd);

    /*
    //useful to add buttons, inputs and other interactive components
    static void AddComponentToFTXUIContainer(const ftxui::Component &gameFlowWindow,
                                             const ftxui::Component &thingToAdd);

    */

    static void AddNewLineToFTXUIContainer(const ftxui::Component &gameFlowWindow);
};

#endif //Game_H
