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
    [[nodiscard]] std::vector<General> getAssignedGenerals() const { return assignedGenerals; }

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
    int k = 0;
    std::vector<int> remainingFights, remainingAllies;
    for (const unsigned long i: battleOrder) {
        std::cout << "Attacking general:\n" << attackingArmy.getAssignedGenerals()[k] << "\nVS\n\nDefending general:\n"
                << this->getAssignedGenerals()[i];
        int fightResult = this->getAssignedGenerals()[i].
                FightWith(attackingArmy.getAssignedGenerals()[k], overallBoost);
        this->evaluateFightOutcome(fightResult, remainingFights, remainingAllies, k, static_cast<int>(i));
        k++;
    }

    //If there are enemies left to fight and we still have allies.
    if (!remainingFights.empty() && !remainingAllies.empty()) {
        std::cout << settlementFightRemainingAttackersText;
        //We still have to defeat the undefeated enemy using remaining generals
        int fightResult;
        unsigned long attempts = 0;
        for (const int remainingFight: remainingFights) {
            //We will attempt to defeat all remaining enemies using our allies.
            //We stop when either they are all defeated or we don't have any allies left.
            do {
                fightResult = this->getAssignedGenerals()[remainingAllies[attempts]].FightWith(
                    attackingArmy.getAssignedGenerals()[remainingFight], overallBoost);
                if (fightResult == 0) {
                    attempts++; //only try going to the next remaining ally if the current loses a battle.
                }
            } while (fightResult == 0 && attempts < remainingAllies.size());
            if (attempts > remainingAllies.size()) {
                //There are no more possible fights, the battle is lost
                return -1;
            }
            //If there are attempts left, we will try going to the next remaining enemy. Automatically done with the for() (if there are any)
        }
        //If we reach this point, the battle is certainly won.
        return 1;
    }
    //If there are enemies left to fight, but we have no allies
    if (!remainingFights.empty() && remainingAllies.empty()) {
        std::cout << settlementFightNoRemainingDefendersText;
        //Battle is lost
        return -1;
    }

    std::cout << settlementFightNoRemainingAttackersText;
    //Otherwise, the battle is won (no enemies left to fight)
    return 1;
    }

#endif