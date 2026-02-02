#include "../header/Captain.h"


void Captain::ApplyStatModifiers(int garrisonOverallBoost, std::vector<int> &selfPowers) const {
    for (unsigned long i = 0; i < selfPowers.size(); i++) {
        selfPowers[i] = selfPowers[i] * captainHandicap + garrisonOverallBoost / garrisonOverallBoostContribution;
    }
}

void Captain::NullifyOrBoost(std::vector<int> &enemyPowers, std::vector<int> &selfPowers) const {
    //First of all, we determine if the generals can nullify stats or get bonuses
    //A captain can't nullify the stats of an enemy!

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

int Captain::InstantWinCheck(std::vector<int> enemyPowers, std::vector<int> selfPowers) const {
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

void Captain::display(std::ostream &os) const {
    os << "Handicap: " << captainHandicap << "\n";
}

Captain::Captain(const std::string &firstName_, const std::string &lastName_, int type_, int rarity_, int melee_,
                 int ranged_, int armour_, int str_, int acc_, int dex_,
                 float captainHandicap_) : Unit(firstName_, lastName_, type_, rarity_, melee_, ranged_, armour_, str_,
                                                acc_, dex_), captainHandicap(captainHandicap_) {
}

Captain CaptainFactory::meleeWeak() {
    return Captain{"Farmer", "Joe", 5, 0, 20, 0, 10, 10, 0, 20, captainInitialHandicapMultiplier};
}

Captain CaptainFactory::meleeMedium() {
    return Captain{"Billy", "Bolly", 5, 1, 50, 0, 20, 30, 0, 30, captainInitialHandicapMultiplier};
}

Captain CaptainFactory::meleeGood() {
    return Captain{"Lirili", "Larila", 5, 2, 80, 0, 50, 60, 0, 60, captainInitialHandicapMultiplier};
}

Captain CaptainFactory::meleeStrong() {
    return Captain{"Strongus", "Mylyus", 5, 3, 95, 0, 85, 90, 0, 95, captainInitialHandicapMultiplier};
}

Captain CaptainFactory::rangedWeak() {
    return Captain{"Prastie", "McGee", 5, 0, 5, 20, 5, 10, 10, 10, captainInitialHandicapMultiplier};
}

Captain CaptainFactory::rangedMedium() {
    return Captain{"Temutus", "Arcus", 5, 1, 5, 60, 15, 25, 25, 25, captainInitialHandicapMultiplier};
}

Captain CaptainFactory::rangedGood() {
    return Captain{"Scopicus", "Armatus", 5, 2, 15, 80, 25, 60, 60, 30, captainInitialHandicapMultiplier};
}

Captain CaptainFactory::rangedStrong() {
    return Captain{"Nimeritus", "Anythingus", 5, 3, 35, 99, 55, 88, 88, 88, captainInitialHandicapMultiplier};
}

Captain CaptainFactory::bothWeak() {
    return Captain{"Brutalicus", "Nonganditus", 5, 0, 10, 10, 10, 10, 10, 10, captainInitialHandicapMultiplier};
}

Captain CaptainFactory::bothMedium() {
    return Captain{"Prapaditus", "Macaroni", 5, 1, 27, 27, 27, 27, 27, 27, captainInitialHandicapMultiplier};
}

Captain CaptainFactory::bothGood() {
    return Captain{"Preponderentus", "Batutix", 5, 2, 55, 55, 55, 55, 55, 55, captainInitialHandicapMultiplier};
}

Captain CaptainFactory::bothStrong() {
    return Captain{"Warius", "Larius", 5, 3, 99, 99, 99, 67, 67, 67, captainInitialHandicapMultiplier};
}







