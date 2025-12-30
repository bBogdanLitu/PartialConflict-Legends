#include "../header/Unit.h"
#include "../header/Game.h"


Unit::Unit(const std::string &firstName_, const std::string &lastName_, int type_, int rarity_, int melee_,
           int ranged_, int armour_, int str_, int acc_, int dex_) : firstName(firstName_),
                                                                     lastName(lastName_), type(type_),
                                                                     rarity(rarity_),
                                                                     melee(melee_), ranged(ranged_),
                                                                     armour(armour_), str(str_), acc(acc_),
                                                                     dex(dex_) {
    StatCap();
    ArmourPowerCalculation();
    RangedPowerCalculation();
    MeleePowerCalculation();
    OverallCalculation();
    UpdatePowers();
    fullName = firstName + " " + lastName;
}

void Unit::StatCap() {
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

void Unit::ArmourPowerCalculation() {
    aPower = dex / 10.0 * (armour + 5);
}

void Unit::RangedPowerCalculation() {
    rPower = acc / 25.0 * str / 25.0 * ranged;
}

void Unit::MeleePowerCalculation() {
    mPower = str / 20.0 * dex / 33.3 * melee;
}

void Unit::OverallCalculation() {
    overallPower = (aPower + rPower + mPower) / 3;
}

void Unit::UpdatePowers() {
    Powers.clear();
    Powers.push_back(mPower);
    Powers.push_back(rPower);
    Powers.push_back(aPower);
}

ftxui::Element Unit::CreateDisplayFightTable(const Unit &enemyUnit, const std::vector<int> &selfEffPowers,
                                             const std::vector<int> &enemyEffPowers) const {
    std::vector<std::vector<std::string> > table1Content, table2Content;

    table1Content.push_back(generalFightTableHeaders); //defender
    table2Content.push_back(generalFightTableHeaders); //attacker

    table1Content.push_back(this->getEffectiveCombatStats(selfEffPowers));
    table2Content.push_back(enemyUnit.getEffectiveCombatStats(enemyEffPowers));

    //Create the tables
    using namespace ftxui;
    auto table1 = Table({table1Content});

    table1.SelectAll().Border(LIGHT);

    //Separate all cells
    table1.SelectAll().SeparatorVertical(LIGHT);

    //Make first row bold with a double border.
    table1.SelectRow(0).Decorate(bold);
    table1.SelectRow(0).SeparatorVertical(LIGHT);
    table1.SelectRow(0).Border(LIGHT);

    //Make the content a different color
    table1.SelectRows(1, -1).DecorateCells(color(generalFightDefenderInfoColor));

    auto table2 = Table({table2Content});

    table2.SelectAll().Border(LIGHT);

    //Separate all cells
    table2.SelectAll().SeparatorVertical(LIGHT);

    //Make first row bold with a double border.
    table2.SelectRow(0).Decorate(bold);
    table2.SelectRow(0).SeparatorVertical(LIGHT);
    table2.SelectRow(0).Border(LIGHT);

    //Make the content a different color
    table2.SelectRows(1, -1).DecorateCells(color(generalFightAttackerInfoColor));

    //Create the document to render
    auto document =
            hbox({
                table1.Render() | center | size(WIDTH, GREATER_THAN, Terminal::Size().dimx / 100 * 45),
                text("VERSUS") | bold | color(Color::Magenta) | center | size(
                    WIDTH, GREATER_THAN, Terminal::Size().dimx / 100 * 10),
                table2.Render() | center | size(WIDTH, GREATER_THAN, Terminal::Size().dimx / 100 * 45),
            });

    return document;
}

void Unit::DisplayFight(const Unit &enemyUnit, const std::vector<int> &selfEffPowers,
                        const std::vector<int> &enemyEffPowers, const ftxui::Component &gameWindow) const {
    //Legacy display
    using namespace ftxui;

    auto document = CreateDisplayFightTable(enemyUnit, selfEffPowers, enemyEffPowers);
    Game::AddElementToFTXUIContainer(gameWindow, document);
}

/*
ftxui::Element Unit::FTXUIDisplayFight(const Unit &enemyUnit, const std::vector<int> &selfEffPowers,
                                       const std::vector<int> &enemyEffPowers) const {
    using namespace ftxui;

    auto document = CreateDisplayFightTable(enemyUnit, selfEffPowers, enemyEffPowers);

    return document;
}
*/
void Unit::display(std::ostream &) const {
}

int Unit::FightWith(const Unit &enemyUnit, const int garrisonOverallBoost, const ftxui::Component &gameWindow) const {
    ///STAGE 1 - FETCH STATS

    int result = 1;
    std::vector<int> enemyPowers = enemyUnit.getPowers(); //can't be const because it's modifiable
    //Copy so that temporary modifications don't have a permanent effect
    std::vector<int> selfPowers = this->getPowers();

    ///STAGE 2 - APPLY MODIFICATIONS

    ApplyStatModifiers(garrisonOverallBoost, selfPowers);

    ///STAGE 3 - NULLIFY/BOOST

    NullifyOrBoost(enemyPowers, selfPowers);

    DisplayFight(enemyUnit, selfPowers, enemyPowers, gameWindow); //to display the effective stats


    //POTENTIAL FIGHT ENDING SCENARIOS DOWN BELOW!
    ///STAGE 4: INSTANT WINS

    int instantWin = InstantWinCheck(enemyPowers, selfPowers);
    //if the defender wins instantly
    if (instantWin == 1) {
        result = 1; //won't be 1 forever (after implementing more consequences to win / lose scenarios)
        return result;
    }
    //if the attacker wins instantly
    if (instantWin == -1) {
        result = 0;
        return result;
    }

    ///STAGE 5: NORMAL WINS (STANDARD FOR EVERY UNIT)

    if (selfPowers[0] > enemyPowers[0]) {
        result = 1;
        return result;
    }
    if (selfPowers[0] < enemyPowers[0]) {
        result = 0;
        return result;
    }
    //Equal melee
    if (selfPowers[1] > enemyPowers[1]) {
        result = 1;
        return result;
    }
    if (selfPowers[1] < enemyPowers[1]) {
        result = 0;
        return result;
    }
    //Equal melee and ranged
    if (selfPowers[2] > enemyPowers[2]) {
        result = 1;
        return result;
    }
    if (selfPowers[2] < enemyPowers[2]) {
        result = 0;
        return result;
    }
    //All equal stats -> whoever gets called (the defender) wins
    return result;
}

/*
int Unit::FTXUIFightWith(const Unit &enemyUnit, const int garrisonOverallBoost,
                         const ftxui::Component &whereToDisplay) const {
    ///STAGE 1 - FETCH STATS

    int result = 1;
    std::vector<int> enemyPowers = enemyUnit.getPowers(); //can't be const because it's modifiable
    //Copy so that temporary modifications don't have a permanent effect
    std::vector<int> selfPowers = this->getPowers();

    ///STAGE 2 - APPLY MODIFICATIONS

    ApplyStatModifiers(garrisonOverallBoost, selfPowers);

    ///STAGE 3 - NULLIFY/BOOST

    NullifyOrBoost(enemyPowers, selfPowers);

    auto whatToDisplay = FTXUIDisplayFight(enemyUnit, selfPowers, enemyPowers); //to display the effective stats
    Game::AddElementToFTXUIContainer(whereToDisplay, whatToDisplay);

    //POTENTIAL FIGHT ENDING SCENARIOS DOWN BELOW!
    ///STAGE 4: INSTANT WINS

    int instantWin = InstantWinCheck(enemyPowers, selfPowers);
    //if the defender wins instantly
    if (instantWin == 1) {
        result = 1; //won't be 1 forever (after implementing more consequences to win / lose scenarios)
        return result;
    }
    //if the attacker wins instantly
    if (instantWin == -1) {
        result = 0;
        return result;
    }

    ///STAGE 5: NORMAL WINS (STANDARD FOR EVERY UNIT)

    if (selfPowers[0] > enemyPowers[0]) {
        result = 1;
        return result;
    }
    if (selfPowers[0] < enemyPowers[0]) {
        result = 0;
        return result;
    }
    //Equal melee
    if (selfPowers[1] > enemyPowers[1]) {
        result = 1;
        return result;
    }
    if (selfPowers[1] < enemyPowers[1]) {
        result = 0;
        return result;
    }
    //Equal melee and ranged
    if (selfPowers[2] > enemyPowers[2]) {
        result = 1;
        return result;
    }
    if (selfPowers[2] < enemyPowers[2]) {
        result = 0;
        return result;
    }
    //All equal stats -> whoever gets called (the defender) wins
    return result;
}
*/

int Unit::getType() const { return type; }

int Unit::getOverallPower() const { return overallPower; }

const std::vector<int> &Unit::getPowers() const { return Powers; }

std::vector<std::string> Unit::getPrintableStats() const {
    std::vector<std::string> printableStats;

    //Convert what I want to show using FTXUI to string
    const std::string typeConverted = std::to_string(type);
    const std::string rarityConverted = std::to_string(rarity);
    const std::string meleeConverted = std::to_string(melee);
    const std::string rangedConverted = std::to_string(ranged);
    const std::string armourConverted = std::to_string(armour);
    const std::string strConverted = std::to_string(str);
    const std::string accConverted = std::to_string(acc);
    const std::string dexConverted = std::to_string(dex);
    const std::string overallPowerConverted = std::to_string(overallPower);

    printableStats.push_back(fullName);
    printableStats.push_back(typeConverted);
    printableStats.push_back(rarityConverted);
    printableStats.push_back(meleeConverted);
    printableStats.push_back(rangedConverted);
    printableStats.push_back(armourConverted);
    printableStats.push_back(strConverted);
    printableStats.push_back(accConverted);
    printableStats.push_back(dexConverted);
    printableStats.push_back(overallPowerConverted);

    return printableStats;
}

std::vector<std::string> Unit::getEffectiveCombatStats(const std::vector<int> &ecs) const {
    std::vector<std::string> combatStats;

    const std::string mPowerConverted = std::to_string(ecs[0]);
    const std::string rPowerConverted = std::to_string(ecs[1]);
    const std::string aPowerConverted = std::to_string(ecs[2]);

    combatStats.push_back(fullName);
    combatStats.push_back(mPowerConverted);
    combatStats.push_back(rPowerConverted);
    combatStats.push_back(aPowerConverted);

    return combatStats;
}