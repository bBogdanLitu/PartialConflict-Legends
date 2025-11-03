#ifndef General_H
#define General_H
#include <string>
#include <ftxui/dom/table.hpp>
#include "Constants.h"
#include "Functions.h"
#include "Unit.h"

class General : public Unit{
private:
    std::string firstName, lastName, fullName;
    int type{}, rarity{}, melee{}, ranged{}, armour{}, str{}, acc{}, dex{}, mPower = 0, rPower = 0, aPower = 0, overallPower = 0;
    std::vector<int> Powers;

    void ApplyStatModifiers(int garrisonOverallBoost, std::vector<int> &selfPowers) const override;

    void NullifyOrBoost(std::vector<int> &enemyPowers, std::vector<int> &selfPowers) const override;

    int InstantWinCheck(std::vector<int> enemyPowers, std::vector<int> selfPowers) const override;

public:
    General(const std::string& firstName_, const std::string& lastName_, int type_, int rarity_,
        int melee_, int ranged_, int armour_, int str_, int acc_, int dex_);

    std::shared_ptr<Unit> clone() const override { return std::make_shared<General>(*this); }
};



#endif //General_H
