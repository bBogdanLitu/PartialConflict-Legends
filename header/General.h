#ifndef General_H
#define General_H
#include <string>
#include <ftxui/dom/table.hpp>
#include "Constants.h"
#include "Functions.h"
#include "Unit.h"

class General : public Unit {
private:
    void ApplyStatModifiers(int garrisonOverallBoost, std::vector<int> &selfPowers) const override;

    void NullifyOrBoost(std::vector<int> &enemyPowers, std::vector<int> &selfPowers) const override;

    [[nodiscard]] int InstantWinCheck(std::vector<int> enemyPowers, std::vector<int> selfPowers) const override;

    void display(std::ostream &) const override;

public:
    General(const std::string &firstName_, const std::string &lastName_, int type_, int rarity_, int melee_,
            int ranged_, int armour_, int str_, int acc_, int dex_);

    [[nodiscard]] std::shared_ptr<Unit> clone() const override { return std::make_shared<General>(*this); }
};


#endif //General_H