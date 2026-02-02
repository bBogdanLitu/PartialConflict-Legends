#ifndef Unit_H
#define Unit_H
#include "Constants.h"
#include "Functions.h"
#include <ftxui/dom/table.hpp>
#include <string>
#include <vector>
#include <iostream>

class Unit {
private:
    std::string firstName, lastName, fullName;
    int index = 0, type, rarity, melee, ranged, armour, str, acc, dex, mPower = 0, rPower = 0, aPower = 0, overallPower
                    = 0;
    std::vector<int> Powers;

    //Applying the designed cap to the stats so the game makes sense
    void StatCap();

    void ArmourPowerCalculation();

    void RangedPowerCalculation();

    void MeleePowerCalculation();

    //very basic mode to balance the generals early
    void OverallCalculation();

    void UpdatePowers();

    [[nodiscard]] ftxui::Element CreateDisplayFightTable(const Unit &enemyUnit, const std::vector<int> &selfEffPowers,
                                                         const std::vector<int> &enemyEffPowers) const;

    void DisplayFight(const Unit &enemyUnit, const std::vector<int> &selfEffPowers,
                      const std::vector<int> &enemyEffPowers, const ftxui::Component &gameWindow) const;

    /*
    ftxui::Element FTXUIDisplayFight(const Unit &enemyUnit, const std::vector<int> &selfEffPowers,
                                     const std::vector<int> &enemyEffPowers) const;
    */

    //Virtual functions used in the NVI (FightWith)
    virtual void ApplyStatModifiers(int garrisonOverallBoost, std::vector<int> &selfPowers) const = 0;

    virtual void NullifyOrBoost(std::vector<int> &enemyPowers, std::vector<int> &selfPowers) const = 0;

    [[nodiscard]] virtual int InstantWinCheck(std::vector<int> enemyPowers, std::vector<int> selfPowers) const = 0;

    virtual void display(std::ostream &) const;

public:
    virtual ~Unit() = default;

    [[nodiscard]] virtual std::shared_ptr<Unit> clone() const = 0;

    Unit(Unit &&other) = default;

    Unit &operator=(Unit &&other) = default;


    Unit(const std::string &firstName_, const std::string &lastName_, int type_, int rarity_,
         int melee_, int ranged_, int armour_, int str_, int acc_, int dex_);


    //Non-virtual interface for combat -> every type of unit can implement its own particularities, but
    //they all follow a common structure
    [[nodiscard]] int FightWith(const Unit &enemyUnit, int garrisonOverallBoost,
                                const ftxui::Component &gameWindow) const;

    //[[nodiscard]] int FTXUIFightWith(const Unit &enemyUnit, int garrisonOverallBoost, const ftxui::Component &whereToDisplay) const;

    void setIndex(int index_);

    [[nodiscard]] int getIndex() const;

    [[nodiscard]] int getType() const;

    [[nodiscard]] const std::string &getFullName();

    [[nodiscard]] int getOverallPower() const;

    [[nodiscard]] const std::vector<int> &getPowers() const;

    [[nodiscard]] std::vector<std::string> getPrintableStats() const;

    [[nodiscard]] std::vector<std::string> getEffectiveCombatStats(const std::vector<int> &ecs) const;


    friend std::ostream &operator<<(std::ostream &os, const Unit &unit) {
        os << "Name: " << unit.fullName << "\nType and rarity: " << unit.type << " " << unit.rarity <<
                "\nMelee, ranged and armour stats: " << unit.melee << " " << unit.ranged << " " << unit.armour <<
                "\nStrength, accuracy and dexterity: " << unit.str << " " << unit.acc << " " << unit.dex <<
                "\nOverall power: " << unit.overallPower << "\n";
        unit.display(os); //to display (eventually) the attributes that each derived class adds
        os << "\n";
        return os;
    }

    friend bool operator<(const Unit &lhs, const Unit &rhs) {
        return lhs.overallPower < rhs.overallPower;
    }

    friend bool operator<=(const Unit &lhs, const Unit &rhs) {
        return !(rhs < lhs);
    }

    friend bool operator>(const Unit &lhs, const Unit &rhs) {
        return rhs < lhs;
    }

    friend bool operator>=(const Unit &lhs, const Unit &rhs) {
        return !(lhs < rhs);
    }

protected:
    Unit(const Unit &other) = default;

    Unit &operator=(const Unit &other) = default;
};


#endif //Unit_H