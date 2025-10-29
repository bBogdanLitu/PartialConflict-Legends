#ifndef Army_H
#define Army_H
#include "General.h"
#include <vector>

#include "Constants.h"

//Can hold up to 3 generals, min of 1.
class Army {
private:
    std::vector<General> assignedGenerals;

    //These attributes shouldn't be modified by the constructor, they are initialized here and updated (eventually) through functions
    int actionPoints = 1, totalOverallPower = 0; //Default
    bool isStationed = true; //All armies must start in a Settlement

    //Will be called whenever a general is added or removed
    int TotalOverallPowerCalculation(const General &modifiedGeneral) {
        totalOverallPower += modifiedGeneral.getOverallPower();
        return totalOverallPower;
    }

    void evaluateFightOutcome(int fightResult, std::vector<int> &remainingFights, std::vector<int> &remainingAllies,
                              int indexOfAttacker, int indexOfDefender) const {
        switch (fightResult) {
            case 0: {
                std::cout << "\nBattle lost by the defender.\n\n";
                remainingFights.push_back(indexOfAttacker); //Save the index of the undefeated attacker
                break;
            }
            case 1: {
                std::cout << "\nBattle won by the defender.\n\n";
                remainingAllies.push_back(static_cast<int>(indexOfDefender));
                //Save the index of the undefeated defender
                break;
            }
            default: {
                std::cout << "Unhandled\n";
            }
        }
    }

public:
    explicit Army(const General &general) {
        if (assignedGenerals.size() < 3) {
            assignedGenerals.push_back(general);
            TotalOverallPowerCalculation(general);
        }
    }

    void AddGeneral(const General &general) {
        assignedGenerals.push_back(general);
    }

    [[nodiscard]] int Attacked(const Army &attackingArmy, int overallBoost, const std::vector<unsigned long> &battleOrder) const;

    [[nodiscard]] int getTotalOverallPower() const { return totalOverallPower; }
    [[nodiscard]] unsigned long getGeneralCount() const { return assignedGenerals.size(); }
    [[nodiscard]] const std::vector<General> &getAssignedGenerals() const { return assignedGenerals; }

    void DisplayArmy() const;

    friend std::ostream& operator<<(std::ostream& os, const Army& army) {
        int k = 0;
        os << "Composition:\n";
        for (const auto &general : army.assignedGenerals) {
            os << k << ".\n" << general;
            k++;
        }
        os << "Action points at the start of every turn: " << army.actionPoints << "\n";
        if (army.isStationed == true) {
            os << "The army is stationed.\n";
        }
        else {
            os << "The army isn't stationed.\n";
        }
        return os;
    }
};

inline int Army::Attacked(const Army &attackingArmy, const int overallBoost,
                          const std::vector<unsigned long> &battleOrder) const {
    std::cout << "This is the battle order:\n";
    int currentEnemy = 0;
    std::vector<int> remainingAttackers, remainingDefenders, surplusDefenders;
    //We assume that every defender and attacker can be surplus (apriori)
    for (int i = 0; i < static_cast<int>(this->getGeneralCount()); i++) {
        surplusDefenders.push_back(i);
    }

    for (const unsigned long currentAlly: battleOrder) {
        //We mark the chosen defender as non-surplus
        surplusDefenders[currentAlly] = -1;
        std::cout << "Attacking general:\n" << attackingArmy.getAssignedGenerals()[currentEnemy] <<
                "\nVS\n\nDefending general:\n"
                << this->getAssignedGenerals()[currentAlly];
        int fightResult = this->getAssignedGenerals()[currentAlly].
                FightWith(attackingArmy.getAssignedGenerals()[currentEnemy], overallBoost);
        //defender combat is boosted by the garrison
        this->evaluateFightOutcome(fightResult, remainingAttackers, remainingDefenders, currentEnemy,
                                   static_cast<int>(currentAlly));
        currentEnemy++;
    }

    //If there are generals that didn't take part because of size difference, we add them to their pools.
    if (this->getGeneralCount() > attackingArmy.getGeneralCount()) {
        for (int surplusDefender: surplusDefenders) {
            if (surplusDefender != -1) {
                remainingDefenders.push_back(surplusDefender);
            }
        }
    } else if (attackingArmy.getGeneralCount() > this->getGeneralCount()) {
        //Because the attackers use a simple order (0, 1, 2) we can simply add what's left to the remaining pool.
        for (unsigned long i = attackingArmy.getGeneralCount() - 1;
             i > armyGeneralsMaximumIndex - (attackingArmy.getGeneralCount() - this->getGeneralCount()); i--) {
            remainingAttackers.push_back(static_cast<int>(i));
        }
    }

    //If there are enemies left to fight and we still have allies.
    if (!remainingAttackers.empty() && !remainingDefenders.empty()) {
        std::cout << settlementFightRemainingAttackersText;
        //We still have to defeat the undefeated enemy using remaining generals
        unsigned long attempts = 0;
        int fightResult = 0;
        for (const int remainingAttacker: remainingAttackers) {
            //We will attempt to defeat all remaining enemies using our allies.
            //We stop when either they are all defeated or we don't have any allies left.
            do {
                std::cout << "Attacking general:\n" << attackingArmy.getAssignedGenerals()[remainingAttacker] <<
                        "\nVS\n\nDefending general:\n"
                        << this->getAssignedGenerals()[remainingDefenders[attempts]] << "\n";
                fightResult = this->getAssignedGenerals()[remainingDefenders[attempts]].FightWith(
                    attackingArmy.getAssignedGenerals()[remainingAttacker], overallBoost);
                if (fightResult == 0) {
                    std::cout << "Battle lost by the defender.\n\n";
                    attempts++; //only try going to the next remaining ally if the current loses a battle.
                } else {
                    std::cout << "Battle won by the defender.\n\n";
                }
            } while (fightResult == 0 && attempts < remainingDefenders.size());
            if (attempts == remainingDefenders.size()) {
                //There are no more possible fights, the battle is lost
                return -1;
            }
            //If there are attempts left, we will try going to the next remaining enemy. Automatically done with the for() (if there are any)
        }
        //If we reach this point, the battle is certainly won.
        return 1;
    }
    //If there are enemies left to fight, but we have no allies
    if (!remainingAttackers.empty() && remainingDefenders.empty()) {
        std::cout << settlementFightNoRemainingDefendersText;
        //Battle is lost
        return -1;
    }

    std::cout << settlementFightNoRemainingAttackersText;
    //Otherwise, the battle is won (no enemies left to fight)
    return 1;
    }

inline void Army::DisplayArmy() const {
    std::vector<std::vector<std::string>> tableContent;
    std::vector<std::string> tableRow;

    tableContent.push_back(armyTableHeaders);

    int count = 0;
    for (const auto& general : assignedGenerals) {
        tableRow = general.getPrintableStats();
        tableRow.emplace(tableRow.begin(), std::to_string(count));
        tableContent.push_back(tableRow);

        count++;
    }

    //Display stuff
    using namespace ftxui;
    auto table = Table({tableContent});

    table.SelectAll().Border(LIGHT);

    //Separate all cells
    table.SelectAll().SeparatorVertical(LIGHT);

    //Make first row bold with a double border.
    table.SelectRow(0).Decorate(bold);
    table.SelectRow(0).SeparatorVertical(LIGHT);
    table.SelectRow(0).Border(DOUBLE);

    //Make the content a different color
    table.SelectRows(1, -1).DecorateCells(color(Color::LightSkyBlue1));

    auto document = table.Render();
    auto screen =
            Screen::Create(Dimension::Fit(document, /*extend_beyond_screen=*/true));
    Render(screen, document);
    screen.Print();
    std::cout << std::endl;
}

#endif