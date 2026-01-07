//
// Created by bogdan on 1/7/26.
//

#include "../header/LocalLeader.h"

void LocalLeader::ApplyStatModifiers(int garrisonOverallBoost, std::vector<int> &selfPowers) const {
    for (unsigned long i = 0; i < selfPowers.size(); i++) {
        selfPowers[i] = selfPowers[i] * battleHandicap + garrisonOverallBoost / garrisonOverallBoostContribution;
    }
}

void LocalLeader::NullifyOrBoost(std::vector<int> &enemyPowers, std::vector<int> &selfPowers) const {
    //First of all, we determine if the generals can nullify stats or get bonuses
    //A local leader can't nullify the stats of an enemy!

    //if aPower is low (will balance later), the enemy gets a boost to ranged and melee power.
    if (selfPowers[2] <= armourMaximumForBonusMultiplier) {
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

int LocalLeader::InstantWinCheck(std::vector<int> enemyPowers, std::vector<int> selfPowers) const {
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

void LocalLeader::display(std::ostream &os) const {
    os << "IncomeMultiplier: " << incomeMultiplier << "\n";
    os << "Handicap: " << battleHandicap << "\n";
}

LocalLeader::LocalLeader(const std::string &firstName_, const std::string &lastName_, int type_, int rarity_,
    int melee_, int ranged_, int armour_, int str_, int acc_, int dex_, int incomeMultiplier_,
    float battleHandicap_) : Unit(firstName_, lastName_, type_, rarity_, melee_, ranged_, armour_, str_, acc_, dex_),
    incomeMultiplier(incomeMultiplier_), battleHandicap(battleHandicap_){
}
