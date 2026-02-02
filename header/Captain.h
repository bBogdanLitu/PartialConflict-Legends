#ifndef Captain_H
#define Captain_H
#include "Unit.h"


class Captain : public Unit {
private:
    float captainHandicap = 0.8f;

    void ApplyStatModifiers(int garrisonOverallBoost, std::vector<int> &selfPowers) const override;

    void NullifyOrBoost(std::vector<int> &enemyPowers, std::vector<int> &selfPowers) const override;

    [[nodiscard]] int InstantWinCheck(std::vector<int> enemyPowers, std::vector<int> selfPowers) const override;

    void display(std::ostream &os) const override;

public:
    Captain(const std::string &firstName_, const std::string &lastName_, int type_, int rarity_, int melee_,
            int ranged_,
            int armour_, int str_, int acc_, int dex_, float captainHandicap_);

    [[nodiscard]] std::shared_ptr<Unit> clone() const override { return std::make_shared<Captain>(*this); }
};

class CaptainFactory {
public:
    static Captain meleeWeak();

    static Captain meleeMedium();

    static Captain meleeGood();

    static Captain meleeStrong();

    static Captain rangedWeak();

    static Captain rangedMedium();

    static Captain rangedGood();

    static Captain rangedStrong();

    static Captain bothWeak();

    static Captain bothMedium();

    static Captain bothGood();

    static Captain bothStrong();
};


#endif //Captain_H