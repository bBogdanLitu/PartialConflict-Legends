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
            EmperorGenerals, Captains, LocalLeaders;
    std::vector<std::shared_ptr<Settlement> > Settlements;
    std::vector<std::shared_ptr<Enemy> > Enemies;
    //I want(ed) to use these somewhere else and that's why they're class attributes (will change when I decide)
    unsigned int ans1 = 0, ans2 = 0, ans3 = 0;
    unsigned long currentTurn = 0;
    long int sarmale = 0; //this is the 'gold' of my game
    ftxui::Component gameWindow;

    void PopulateEnemies(std::ifstream enemiesJson);

    void PopulateGenerals(std::ifstream generalsJson);

    void PopulateSettlements(std::ifstream settlementsJson);

    void PopulateControlPoints(std::ifstream controlPointsJson);

    void PopulateCaptains(std::ifstream captainsJson);

    void PopulateLocalLeaders(std::ifstream localLeadersJson);

    void InitializeWarlordArmies() const;

    void CheckGenerals() const;

    void ShowPlayerGenerals() const;

    [[nodiscard]] ftxui::Table CreateStartingGeneralsTable() const;

    void DisplayStartingGenerals() const;

    [[nodiscard]] ftxui::Element FTXUIDisplayStaringGenerals() const;

    [[nodiscard]] ftxui::Table CreatePlayerAdditionalGeneralsTable() const;

    [[nodiscard]] ftxui::Element FTXUIDisplayAdditionalPlayerGenerals() const;

    [[nodiscard]] ftxui::Table CreateCaptainsTable() const;

    [[nodiscard]] ftxui::Element FTXUIDisplayCaptains() const;

    static void FTXUIDisplaySettlementAndArmy(const ftxui::Component &whereToDisplay, const Settlement &settlement);

    static void FTXUIDisplayOnlyArmyFromSettlement(const ftxui::Component &whereToDisplay,
                                                   const Settlement &settlement);

    void FTXUIDisplayOnlyPlayerSettlementsWithArmies(const ftxui::Component &whereToDisplay) const;

    void ResetArmiesActionPoints() const;

    void CollectIncomeFromSettlements();

    //void TickAttackCountdown();

    void NextTurn();

    static void ReplaceAllButtonsWithAnother(const ftxui::Component &container, const ftxui::Component &button);

public:
    int Start();

    //useful to add text, tables and other non-interactive elements
    static void AddElementToFTXUIContainer(const ftxui::Component &gameFlowWindow, const ftxui::Element &thingToAdd);


    static void AddNewLineToFTXUIContainer(const ftxui::Component &gameFlowWindow);

    ~Game();
};

#endif //Game_H
