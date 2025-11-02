#ifndef Captain_H
#define Captain_H
#include "Unit.h"


class Captain : public Unit {
private:
    float captainHandicap = 0.8;

    void ApplyStatModifiers(int garrisonOverallBoost, std::vector<int> &selfPowers) const override;

    void NullifyOrBoost(std::vector<int> enemyPowers, std::vector<int> selfPowers) const override;

    int InstantWinCheck(std::vector<int> enemyPowers, std::vector<int> selfPowers) const override;

public:
    Captain(const std::string &firstName_, const std::string &lastName_, int type_, int rarity_,
            int melee_, int ranged_, int armour_, int str_, int acc_, int dex_, float captainHandicap_);

    std::shared_ptr<Unit> clone() const override { return std::make_shared<Captain>(*this); }
};


#endif //Captain_H
