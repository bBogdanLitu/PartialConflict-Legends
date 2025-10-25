#ifndef General_H
#define General_H
#include <string>

#include "Constants.h"
#include "Formulas.h"

class General {
private:
    std::string firstName, lastName, fullName;
    int type, rarity, melee, ranged, armour, str, acc, dex, mPower = 0, rPower = 0, aPower = 0, overallPower = 0;
    std::vector<int> Powers;

    //Applying the designed cap to the stats so the game makes sense
    void StatCap();
    void ArmourPowerCalculation() {
        aPower = dex / 10.0 * (armour + 5);
    }
    void RangedPowerCalculation() {
        rPower = acc / 25.0 * str / 25.0 * ranged;
    }
    void MeleePowerCalculation() {
        mPower = str / 20.0 * dex / 33.3 * melee;
    }
    //very basic mode to balance the generals early
    void OverallCalculation() {
        overallPower = (aPower + rPower + mPower) / 3;
    }

    void UpdatePowers() {
        Powers.clear();
        Powers.push_back(mPower);
        Powers.push_back(rPower);
        Powers.push_back(aPower);
    }

public:
    General(const std::string& firstName_, const std::string& lastName_, int type_, int rarity_,
        int melee_, int ranged_, int armour_, int str_, int acc_, int dex_);

    //Combat function
    int FightWith(const General &enemyGeneral, int overallBoost) const;

    [[nodiscard]] int getType() const { return type; }
    [[nodiscard]] int getOverallPower() const { return overallPower; }
    [[nodiscard]] std::vector<int> getPowers() const { return Powers; }


    friend std::ostream& operator<<(std::ostream& os, const General& general) {
        os << "Name: " << general.fullName << "\nType and rarity: " << general.type << " " << general.rarity <<
            "\nMelee, ranged and armour stats: " << general.melee << " " << general.ranged << " " << general.armour <<
                "\nStrength, accuracy and dexterity: " << general.str << " " << general.acc << " " << general.dex <<
                    "\nOverall power: " <<general.overallPower << "\n";
        return os;

    }
};

inline General::General(const std::string &firstName_, const std::string &lastName_, int type_, int rarity_, int melee_,
                        int ranged_, int armour_, int str_, int acc_, int dex_) : firstName(firstName_),
    lastName(lastName_), type(type_), rarity(rarity_),
    melee(melee_), ranged(ranged_), armour(armour_), str(str_), acc(acc_), dex(dex_) {
    StatCap();
    ArmourPowerCalculation();
    RangedPowerCalculation();
    MeleePowerCalculation();
    OverallCalculation();
    UpdatePowers();
    fullName = firstName + " " + lastName;
}

inline void General::StatCap() {
    if (melee >= 100) melee = 99;
    else if (melee <= -1) melee = 0;
    if (ranged >= 100) ranged = 99;
    else if (ranged <= -1) ranged = 0;
    if (armour >= 100) armour = 99;
    else if (armour <= -1) armour = 0;
    if (str >= 100) str = 99;
    else if (str <= -1) str = 0;
    if (acc >= 100) acc = 99;
    else if (acc <= -1) acc = 0;
    if (dex >= 100) dex = 99;
    else if (dex <= -1) dex = 0;
}

inline int General::FightWith(const General &enemyGeneral, const int overallBoost) const {
    int result = 1;
    std::vector<int> enemyPowers = enemyGeneral.getPowers();
    std::vector<int> selfPowers = this->getPowers();
    //Copy so that temporary modifications don't have a permanent effect

    //Apply garrison boost to defending general
    for (unsigned long i = 0; i < selfPowers.size(); i++) {
        selfPowers[i] += overallBoost / garrisonOverallBoostContribution;
    }

    //First of all, we determine if the generals can nullify stats or get bonuses
    //armour -> ranged (if armour >= 1000, no ranged power can penetrate it)
    if (selfPowers[2] >= armourToRangedNullifierCertain) {
        enemyPowers[1] = 0;
    }
    //if aPower is at least 416, there might be a chance to nullify ranged using the formula in Formulas.h
    else if (selfPowers[2] >= armourToRangedNullifierPossible && ArmourToRangedNullification(
                 enemyPowers[1], selfPowers[2]) == true) {
        enemyPowers[1] = 0;
    }
    //if aPower is low (will balance later), the enemy gets a boost to ranged and melee power.
    else if (selfPowers[2] <= armourMaximumForBonusMultiplier) {
        enemyPowers[0] *= meleeBonusMultiplierForLowArmour;
        enemyPowers[1] *= rangedBonusMultiplierForLowArmour;
    }

    if (enemyPowers[2] >= armourToRangedNullifierCertain) {
        selfPowers[1] = 0;
    } else if (enemyPowers[2] >= armourToRangedNullifierPossible && ArmourToRangedNullification(
                   selfPowers[1], enemyPowers[2]) == true) {
        selfPowers[1] = 0;
    } else if (enemyPowers[2] <= armourMaximumForBonusMultiplier) {
        selfPowers[0] *= meleeBonusMultiplierForLowArmour;
        selfPowers[1] *= rangedBonusMultiplierForLowArmour;
    }

    //If there is a very talented ranged vs a mediocre melee with lesser rangedPower, instant win for the ranged.
    if (enemyPowers[1] > rangedToMeleeOverpowerFixedAddition && RangedToMeleeInstantWin(
            enemyPowers[1], selfPowers[0], selfPowers[1]) == true) {
        result = 0; //loss
        return result;
    }
    if (selfPowers[1] > rangedToMeleeOverpowerFixedAddition && RangedToMeleeInstantWin(
            selfPowers[1], enemyPowers[0], enemyPowers[1]) == true) {
        result = 1; //win
        return result;
    }

    if (selfPowers[0] > enemyPowers[0]) {
        result = 1;
        return result;
    }
    if (selfPowers[0] < enemyPowers[0]) {
        result = 0;
        return result;
    }
    //Equal melee
    if (selfPowers[1] > enemyPowers[1]) {
        result = 1;
        return result;
    }
    if (selfPowers[1] < enemyPowers[1]) {
        result = 0;
        return result;
    }
    //Equal melee and ranged
    if (selfPowers[2] > enemyPowers[2]) {
        result = 1;
        return result;
    }
    if (selfPowers[2] < enemyPowers[2]) {
        result = 0;
        return result;
    }
    //All equal stats -> whoever gets called (the defender) wins
    return result;
    }


#endif //General_H
