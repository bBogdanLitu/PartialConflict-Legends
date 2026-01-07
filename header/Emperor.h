#ifndef EMPEROR_H
#define EMPEROR_H
#include "Unit.h"


class Emperor : public Unit {
private:
    //this is hard-coded
    float emperorChadBuff = 7.f;

    void ApplyStatModifiers(int garrisonOverallBoost, std::vector<int> &selfPowers) const override;

    void NullifyOrBoost(std::vector<int> &enemyPowers, std::vector<int> &selfPowers) const override;

    [[nodiscard]] int InstantWinCheck(std::vector<int> enemyPowers, std::vector<int> selfPowers) const override;

    void display(std::ostream &) const override;

public:
    Emperor(const std::string &firstName_, const std::string &lastName_, int type_, int rarity_,
            int melee_, int ranged_, int armour_, int str_, int acc_, int dex_);

    [[nodiscard]] std::shared_ptr<Unit> clone() const override { return std::make_shared<Emperor>(*this); }
};


#endif //EMPEROR_H
