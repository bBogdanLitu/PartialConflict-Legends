#ifndef Garrison_H
#define Garrison_H

//Immovable force that is the base defence of a (sufficiently upgraded) Settlement.
class Garrison {
private:
    int powerLevel, mPower = 0, rPower = 0, aPower = 0, overallPower = 0;

    void PowerCalculation();

public:
    explicit Garrison(const int powerLevel_) : powerLevel(powerLevel_) {
        PowerCalculation();
    }

    friend std::ostream&  operator<<(std::ostream& os, const Garrison& garrison) {
        os << "This garrison has an overall of: "<< garrison.overallPower <<"\n";
        return os;
    }
};

inline void Garrison::PowerCalculation() {
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

#endif