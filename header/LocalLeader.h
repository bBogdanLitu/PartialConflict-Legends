#ifndef LocalLeader_H
#define LocalLeader_H

#include "Unit.h"

//A high-class individual that boosts the income of a settlement, with a varying efficiency in battle
class LocalLeader : public Unit {
    int incomeMultiplier;
    float battleHandicap;

    void ApplyStatModifiers(int garrisonOverallBoost, std::vector<int> &selfPowers) const override;

    void NullifyOrBoost(std::vector<int> &enemyPowers, std::vector<int> &selfPowers) const override;

    [[nodiscard]] int InstantWinCheck(std::vector<int> enemyPowers, std::vector<int> selfPowers) const override;

    void display(std::ostream &os) const override;

public:
    LocalLeader(const std::string& firstName_, const std::string& lastName_, int type_, int rarity_,
        int melee_, int ranged_, int armour_, int str_, int acc_, int dex_, int incomeMultiplier_, float battleHandicap_);

    [[nodiscard]] std::shared_ptr<Unit> clone() const override { return std::make_shared<LocalLeader>(*this); }
};


#endif //LocalLeader_H