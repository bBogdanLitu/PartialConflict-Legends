#ifndef Formulas_H
#define Formulas_H
#include "Constants.h"

//Combat formulas
inline bool ArmourToRangedNullification(const int rangedPower, const int armourPower) {
    bool result = false;
    if (armourToRangedNullifierPossible + (rangedPower - armourToRangedNullifierMinimumRPower) /
        armourToRangedNullifierMInFormula > armourPower) {
        result = true;
    }
    return result;
}

inline bool RangedToMeleeInstantWin(const int rangedPower, const int meleePower, const int otherRangedPower) {
    bool result = false;
    if (rangedPower > meleePower * rangedToMeleeOverpowerRequiredMultiplier + rangedToMeleeOverpowerFixedAddition &&
        rangedPower >= otherRangedPower) {
        result = true;
    }
    return result;
}

inline bool RangedToMeleeDebuff(const int rangedPower, const int meleePower) {
    bool result = false;
    if (rangedPower > meleePower * rangedToMeleeDebuffRequiredMultiplier + rangedToMeleeDebuffFixedAddition) {
        result = true;
    }
    return result;
}

/*inline bool RangedToMeleeDebuff(const int rangedPower, const int meleePower) {
    bool result = false;
    if (rangedPower > (meleePower))

    return result;
}*/

#endif
