#include "../header/General.h"

void General::ApplyStatModifiers(int garrisonOverallBoost, std::vector<int> &selfPowers) const {
    for (unsigned long i = 0; i < selfPowers.size(); i++) {
        selfPowers[i] += garrisonOverallBoost / garrisonOverallBoostContribution;
    }
}

void General::NullifyOrBoost(std::vector<int> &enemyPowers, std::vector<int> &selfPowers) const {
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

    //If there is a sizeable difference between one's ranged vs the other's melee, melee gets debuffed
    if (enemyPowers[1] > rangedToMeleeDebuffFixedAddition && RangedToMeleeDebuff(enemyPowers[1], selfPowers[0])) {
        selfPowers[0] *= rangedToMeleeDebuffMultiplier;
    }
    if (selfPowers[1] > rangedToMeleeDebuffFixedAddition && RangedToMeleeDebuff(selfPowers[1], enemyPowers[0])) {
        enemyPowers[0] *= rangedToMeleeDebuffMultiplier;
    }
}

int General::InstantWinCheck(std::vector<int> enemyPowers, std::vector<int> selfPowers) const {
    int result = 0;
    //If there is a very talented ranged vs a mediocre melee with lesser rangedPower, instant win for the ranged.
    if (enemyPowers[1] > rangedToMeleeOverpowerFixedAddition && RangedToMeleeInstantWin(
            enemyPowers[1], selfPowers[0], selfPowers[1]) == true) {
        result = -1; //instant loss
        return result;
    }
    if (selfPowers[1] > rangedToMeleeOverpowerFixedAddition && RangedToMeleeInstantWin(
            selfPowers[1], enemyPowers[0], enemyPowers[1]) == true) {
        result = 1; //instant win
        return result;
    }
    return result; //no instant result!
}

//No special attributes to display yet
void General::display(std::ostream &) const {}

General::General(const std::string &firstName_, const std::string &lastName_, int type_, int rarity_, int melee_,
                 int ranged_, int armour_, int str_, int acc_, int dex_) : Unit(firstName_, lastName_, type_, rarity_,
                                                                                melee_, ranged_, armour_, str_, acc_, dex_) {
}



