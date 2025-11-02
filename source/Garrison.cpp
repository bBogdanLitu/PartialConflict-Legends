#include "../header/Garrison.h"

void Garrison::PowerCalculation() {
    switch (powerLevel) {
        case 0: {
            mPower = rPower = aPower = 250;
            break;
        }
        case 1: {
            mPower = rPower = aPower = 500;
            break;
        }
        case 2: {
            mPower = rPower = aPower = 750;
            break;
        }
        default: {
            mPower = rPower = aPower = 25; //a starting garrison for every settlement
        }
    }
    overallPower = (mPower + rPower + aPower) / 3;
}

Garrison::Garrison(const int powerLevel_) : powerLevel(powerLevel_) {
    PowerCalculation();
}

int Garrison::GetOverallPower() const { return overallPower; }

int Garrison::DirectlyAttacked(const Army &attackingArmy) const {
    int actualPower = this->overallPower / garrisonOverallBoostContribution;
    if (actualPower < attackingArmy.getTotalOverallPower()) {
        return 1; //The battle is won by the defending garrison
    }
    return -1; //The battle is won by the attacking army
}