#ifndef Garrison_H
#define Garrison_H
#include "Army.h"

//Immovable force that is the base defence of a (sufficiently upgraded) Settlement.
class Garrison {
private:
    int powerLevel, mPower = 0, rPower = 0, aPower = 0, overallPower = 0;

    void PowerCalculation();

public:
    explicit Garrison(const int powerLevel_);

    [[nodiscard]] int GetOverallPower() const;

    //If there is no stationed army to help the defence of the settlement
    [[nodiscard]] int DirectlyAttacked(const Army &attackingArmy) const;

    friend std::ostream&  operator<<(std::ostream& os, const Garrison& garrison) {
        os << "This garrison has an overall of: "<< garrison.overallPower <<"\n";
        return os;
    }

    friend bool operator==(const Garrison &lhs, const Garrison &rhs) {
        return lhs.powerLevel == rhs.powerLevel
               && lhs.mPower == rhs.mPower
               && lhs.rPower == rhs.rPower
               && lhs.aPower == rhs.aPower
               && lhs.overallPower == rhs.overallPower;
    }

    friend bool operator!=(const Garrison &lhs, const Garrison &rhs) {
        return !(lhs == rhs);
    }
};


#endif
