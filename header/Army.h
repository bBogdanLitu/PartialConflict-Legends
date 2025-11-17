#ifndef Army_H
#define Army_H
#include "General.h"
#include "Unit.h"
#include <vector>
#include "Constants.h"

//Can hold up to 3 generals, min of 1.
class Army {
private:
    std::vector<std::shared_ptr<Unit> > assignedUnits;

    //These attributes shouldn't be modified by the constructor, they are initialized here and updated (eventually) through functions
    int defaultActionPoints = 1, currentActionPoints = 0, totalOverallPower = 0; //Default
    bool isStationed = true; //All armies must start in a Settlement

    //Will be called whenever a general is added or removed
    int TotalOverallPowerCalculation(const std::shared_ptr<Unit> &modifiedUnit);

    static void evaluateFightOutcome(int fightResult, std::vector<int> &remainingFights,
                                     std::vector<int> &remainingAllies,
                                     int indexOfAttacker, int indexOfDefender);

public:
    //explicit Army(const General &general);

    explicit Army(const std::shared_ptr<Unit> &unit);

    void AddUnit(const std::shared_ptr<Unit> &unit);

    [[nodiscard]] int Attacked(const Army &attackingArmy, int overallBoost, const std::vector<unsigned long> &
                               battleOrder) const;

    [[nodiscard]] int FTXUIAttacked(const Army &attackingArmy, int overallBoost,
                                    const std::vector<unsigned long> &battleOrder, const ftxui::Component &whereToDisplay) const;

    [[nodiscard]] int getTotalOverallPower() const;


    [[nodiscard]] unsigned long getUnitCount() const;

    int useActionPoint();

    void resetActionPoints();

    [[nodiscard]] const std::vector<std::shared_ptr<Unit> > &getAssignedUnits() const;

    [[nodiscard]] ftxui::Table CreateArmyTable() const;

    void DisplayArmy() const;

    [[nodiscard]] ftxui::Element FTXUIDisplayArmy() const;

    //cc
    Army(const Army &other);

    //op= cu swap
    Army &operator=(Army other);

    friend void swap(Army &first, Army &second);

    friend std::ostream& operator<<(std::ostream& os, const Army& army) {
        int k = 0;
        os << "Composition:\n";
        for (const auto &unit: army.assignedUnits) {
            os << k << ".\n" << unit;
            k++;
        }
        os << "Action points at the start of every turn: " << army.defaultActionPoints << "\n";
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
