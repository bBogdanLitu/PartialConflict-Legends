#ifndef General_H
#define General_H
#include <string>

class General {
private:
    std::string firstName, lastName, fullName;
    int type, rarity, melee, ranged, armour, str, acc, dex, mPower = 0, rPower = 0, aPower = 0, overallPower = 0;

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


public:
    General(const std::string& firstName_, const std::string& lastName_, int type_, int rarity_,
        int melee_, int ranged_, int armour_, int str_, int acc_, int dex_);

    [[nodiscard]] int getType() const { return type; }
    [[nodiscard]] int getOverallPower() const { return overallPower; }


    friend std::ostream& operator<<(std::ostream& os, const General& general) {
        os << "Name: " << general.fullName << "\nType and rarity: " << general.type << " " << general.rarity <<
            "\nMelee, ranged and armour stats: " << general.melee << " " << general.ranged << " " << general.armour <<
                "\nStrength, accuracy and dexterity: " << general.str << " " << general.acc << " " << general.dex <<
                    "\nOverall power: " <<general.overallPower << "\n";
        return os;

    }
};


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

inline General::General(const std::string &firstName_, const std::string &lastName_, int type_, int rarity_, int melee_,
    int ranged_, int armour_, int str_, int acc_, int dex_):
    firstName(firstName_), lastName(lastName_), type(type_), rarity(rarity_),
    melee(melee_), ranged(ranged_), armour(armour_), str(str_), acc(acc_), dex(dex_) {
    StatCap();
    ArmourPowerCalculation();
    RangedPowerCalculation();
    MeleePowerCalculation();
    OverallCalculation();
    fullName = firstName + " " + lastName;
}


#endif //General_H
