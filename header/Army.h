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

    [[nodiscard]] int Attacked(const Army &attackingArmy, const int overallBoost,
                               const std::vector<unsigned long> &battleOrder) const {
        std::cout << "This is the battle order:\n";
        for (const unsigned long i: battleOrder) {
            std::cout << i << "\n";
        }
        int boostedOverallPower = this->totalOverallPower + overallBoost / garrisonOverallBoostContribution;
        if (boostedOverallPower >= attackingArmy.getTotalOverallPower()) {
            return 1; //The battle is won by the defending army
        }
        return -1; //The battle is won by the attacking army
    }

    [[nodiscard]] int getTotalOverallPower() const { return totalOverallPower; }
    [[nodiscard]] unsigned long getGeneralCount() const { return assignedGenerals.size(); }

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

#endif