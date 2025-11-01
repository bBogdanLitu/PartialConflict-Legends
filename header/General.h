#ifndef General_H
#define General_H
#include <string>
#include <ftxui/dom/table.hpp>
#include "Constants.h"
#include "Functions.h"

class General {
private:
    std::string firstName, lastName, fullName;
    int type, rarity, melee, ranged, armour, str, acc, dex, mPower = 0, rPower = 0, aPower = 0, overallPower = 0;
    std::vector<int> Powers;

    //Applying the designed cap to the stats so the game makes sense
    void StatCap();
    void ArmourPowerCalculation();

    void RangedPowerCalculation();

    void MeleePowerCalculation();

    //very basic mode to balance the generals early
    void OverallCalculation();

    void UpdatePowers();

    void DisplayFight(const General &enemyGeneral, const std::vector<int> &selfEffPowers,
                      const std::vector<int> &enemyEffPowers) const;

public:
    General(const std::string& firstName_, const std::string& lastName_, int type_, int rarity_,
        int melee_, int ranged_, int armour_, int str_, int acc_, int dex_);

    //Combat function
    [[nodiscard]] int FightWith(const General &enemyGeneral, int overallBoost) const;

    [[nodiscard]] int getType() const;

    [[nodiscard]] int getOverallPower() const;

    [[nodiscard]] const std::vector<int> &getPowers() const;

    [[nodiscard]] std::vector<std::string> getPrintableStats() const;

    [[nodiscard]] std::vector<std::string> getEffectiveCombatStats(const std::vector<int>& ecs) const;


    friend std::ostream& operator<<(std::ostream& os, const General& general) {
        os << "Name: " << general.fullName << "\nType and rarity: " << general.type << " " << general.rarity <<
            "\nMelee, ranged and armour stats: " << general.melee << " " << general.ranged << " " << general.armour <<
                "\nStrength, accuracy and dexterity: " << general.str << " " << general.acc << " " << general.dex <<
                    "\nOverall power: " <<general.overallPower << "\n";
        return os;

    }
};



#endif //General_H
