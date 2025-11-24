#include "../header/Army.h"
#include "../header/Game.h"

int Army::TotalOverallPowerCalculation(const std::shared_ptr<Unit> &unit) {
    totalOverallPower += unit->getOverallPower();
    return totalOverallPower;
}

void Army::evaluateFightOutcome(int fightResult, std::vector<int> &remainingFights,
                                std::vector<int> &remainingAllies, int indexOfAttacker, int indexOfDefender) {
    switch (fightResult) {
        case 0: {
            OutputFTXUIText("\nFight won by the attacker.\n\n", generalFightAttackerWinColor);
            //Save the index of the undefeated attacker
            remainingFights.push_back(indexOfAttacker);
            break;
        }
        case 1: {
            OutputFTXUIText("\nFight won by the defender.\n\n", generalFightDefenderWinColor);
            //Save the index of the undefeated defender
            remainingAllies.push_back(static_cast<int>(indexOfDefender));
            break;
        }
        default: {
            std::cout << "Unhandled\n";
        }
    }
}
/*
void Army::FTXUIEvaluateFightOutcome(int fightResult, std::vector<int> &remainingFights,
                                     std::vector<int> &remainingAllies, int indexOfAttacker, int indexOfDefender,
                                     const ftxui::Component &whereToDisplay) {
    switch (fightResult) {
        case 0: {
            //OutputFTXUIText("\nFight won by the attacker.\n\n", generalFightAttackerWinColor);
            Game::AddElementToFTXUIContainer(whereToDisplay,
                                             ftxui::paragraph("Fight won by the attacker.") | color(
                                                 generalFightAttackerWinColor));
            //Save the index of the undefeated attacker
            remainingFights.push_back(indexOfAttacker);
            break;
        }
        case 1: {
            //OutputFTXUIText("\nFight won by the defender.\n\n", generalFightDefenderWinColor);
            Game::AddElementToFTXUIContainer(whereToDisplay,
                                             ftxui::paragraph("Fight won by the defender.") | color(
                                                 generalFightDefenderWinColor));
            //Save the index of the undefeated defender
            remainingAllies.push_back(static_cast<int>(indexOfDefender));
            break;
        }
        default: {
            std::cout << "Unhandled\n";
        }
    }
}
*/

Army::Army(const std::shared_ptr<Unit> &unit) {
    if (assignedUnits.size() < 3) {
        assignedUnits.push_back(unit);
        TotalOverallPowerCalculation(unit);
    }
}

void Army::AddUnit(const std::shared_ptr<Unit> &unit) {
    assignedUnits.push_back(unit);
}

int Army::Attacked(const Army &attackingArmy, const int overallBoost,
                   const std::vector<unsigned long> &battleOrder) const {
    OutputFTXUIText("\nThe battle will now begin.\n", gameAnnouncementsColor);
    int currentEnemy = 0;
    std::vector<int> remainingAttackers, remainingDefenders, surplusDefenders;
    //We assume that every defender and attacker can be surplus (apriori)
    for (int i = 0; i < static_cast<int>(this->getUnitCount()); i++) {
        surplusDefenders.push_back(i);
    }

    for (const unsigned long currentAlly: battleOrder) {
        //We mark the chosen defender as non-surplus
        surplusDefenders[currentAlly] = -1;
        /*int fightResult = this->getAssignedGenerals()[currentAlly].
                FightWith(attackingArmy.getAssignedGenerals()[currentEnemy], overallBoost);*/
        int fightResult = this->getAssignedUnits()[currentAlly]->
                FightWith(*attackingArmy.getAssignedUnits()[currentEnemy], overallBoost);
        //defender combat is boosted by the garrison
        this->evaluateFightOutcome(fightResult, remainingAttackers, remainingDefenders, currentEnemy,
                                   static_cast<int>(currentAlly));
        currentEnemy++;
    }

    //If there are generals that didn't take part because of size difference, we add them to their pools.
    if (this->getUnitCount() > attackingArmy.getUnitCount()) {
        for (int surplusDefender: surplusDefenders) {
            if (surplusDefender != -1) {
                remainingDefenders.push_back(surplusDefender);
            }
        }
    } else if (attackingArmy.getUnitCount() > this->getUnitCount()) {
        //Because the attackers use a simple order (0, 1, 2) we can simply add what's left to the remaining pool.
        for (unsigned long i = attackingArmy.getUnitCount() - 1;
             i > armyGeneralsMaximumIndex - (attackingArmy.getUnitCount() - this->getUnitCount()); i--) {
            remainingAttackers.push_back(static_cast<int>(i));
        }
    }

    //If there are enemies left to fight and we still have allies.
    if (!remainingAttackers.empty() && !remainingDefenders.empty()) {
        OutputFTXUIText(settlementFightRemainingAttackersText, gameAnnouncementsColor);

        //We still have to defeat the undefeated enemy using remaining generals
        unsigned long attempts = 0;
        int fightResult = 0;
        for (const int remainingAttacker: remainingAttackers) {
            //We will attempt to defeat all remaining enemies using our allies.
            //We stop when either they are all defeated or we don't have any allies left.
            do {
                fightResult = this->getAssignedUnits()[remainingDefenders[attempts]]->FightWith(
                    *attackingArmy.getAssignedUnits()[remainingAttacker], overallBoost);
                if (fightResult == 0) {
                    OutputFTXUIText("\nFight won by the attacker.\n\n", generalFightAttackerWinColor);
                    attempts++; //only try going to the next remaining ally if the current loses a battle.
                } else {
                    OutputFTXUIText("\nFight won by the defender.\n\n", generalFightDefenderWinColor);
                }
            } while (fightResult == 0 && attempts < remainingDefenders.size());
            if (attempts == remainingDefenders.size()) {
                //There are no more possible fights, the battle is lost
                return -1;
            }
            //If there are attempts left, we will try going to the next remaining enemy. Automatically done with the for() (if there are any)
        }
        //If we reach this point, the battle is certainly won.
        return 1;
    }
    //If there are enemies left to fight, but we have no allies
    if (!remainingAttackers.empty() && remainingDefenders.empty()) {
        OutputFTXUIText(settlementFightNoRemainingDefendersText, generalFightAttackerWinColor);
        //Battle is lost
        return -1;
    }

    OutputFTXUIText(settlementFightNoRemainingAttackersText, generalFightDefenderWinColor);
    //Otherwise, the battle is won (no enemies left to fight)
    return 1;
}


/*
int Army::FTXUIAttacked(const Army &attackingArmy, int overallBoost,
                        const std::vector<unsigned long> &battleOrder, const ftxui::Component &whereToDisplay) const {
    //OutputFTXUIText("\nThe battle will now begin.\n", gameAnnouncementsColor);
    Game::AddElementToFTXUIContainer(whereToDisplay,
                                     ftxui::paragraph("The battle will now begin.") | ftxui::color(
                                         gameAnnouncementsColor));
    int currentEnemy = 0;
    std::vector<int> remainingAttackers, remainingDefenders, surplusDefenders;
    //We assume that every defender and attacker can be surplus (apriori)
    for (int i = 0; i < static_cast<int>(this->getUnitCount()); i++) {
        surplusDefenders.push_back(i);
    }

    for (const unsigned long currentAlly: battleOrder) {
        //We mark the chosen defender as non-surplus
        surplusDefenders[currentAlly] = -1;
        int fightResult = this->getAssignedGenerals()[currentAlly].
                FightWith(attackingArmy.getAssignedGenerals()[currentEnemy], overallBoost);
        int fightResult = this->getAssignedUnits()[currentAlly]->
                FTXUIFightWith(*attackingArmy.getAssignedUnits()[currentEnemy], overallBoost, whereToDisplay);
        //defender combat is boosted by the garrison
        Army::FTXUIEvaluateFightOutcome(fightResult, remainingAttackers, remainingDefenders, currentEnemy,
                                   static_cast<int>(currentAlly), whereToDisplay);
        currentEnemy++;
    }

    //If there are generals that didn't take part because of size difference, we add them to their pools.
    if (this->getUnitCount() > attackingArmy.getUnitCount()) {
        for (int surplusDefender: surplusDefenders) {
            if (surplusDefender != -1) {
                remainingDefenders.push_back(surplusDefender);
            }
        }
    } else if (attackingArmy.getUnitCount() > this->getUnitCount()) {
        //Because the attackers use a simple order (0, 1, 2) we can simply add what's left to the remaining pool.
        for (unsigned long i = attackingArmy.getUnitCount() - 1;
             i > armyGeneralsMaximumIndex - (attackingArmy.getUnitCount() - this->getUnitCount()); i--) {
            remainingAttackers.push_back(static_cast<int>(i));
        }
    }

    //If there are enemies left to fight and we still have allies.
    if (!remainingAttackers.empty() && !remainingDefenders.empty()) {
        //OutputFTXUIText(settlementFightRemainingAttackersText, gameAnnouncementsColor);
        Game::AddElementToFTXUIContainer(whereToDisplay, ftxui::paragraph(settlementFightRemainingAttackersText) | color(gameAnnouncementsColor));

        //We still have to defeat the undefeated enemy using remaining generals
        unsigned long attempts = 0;
        int fightResult = 0;
        for (const int remainingAttacker: remainingAttackers) {
            //We will attempt to defeat all remaining enemies using our allies.
            //We stop when either they are all defeated or we don't have any allies left.
            do {
                fightResult = this->getAssignedUnits()[remainingDefenders[attempts]]->FTXUIFightWith(
                    *attackingArmy.getAssignedUnits()[remainingAttacker], overallBoost, whereToDisplay);
                if (fightResult == 0) {
                    Game::AddElementToFTXUIContainer(whereToDisplay,
                                                     ftxui::paragraph("Fight won by the attacker.") | color(
                                                         generalFightAttackerWinColor));
                    attempts++; //only try going to the next remaining ally if the current loses a battle.
                } else {
                    Game::AddElementToFTXUIContainer(whereToDisplay,
                                                     ftxui::paragraph("Fight won by the defender.") | color(
                                                         generalFightDefenderWinColor));
                }
            } while (fightResult == 0 && attempts < remainingDefenders.size());
            if (attempts == remainingDefenders.size()) {
                //There are no more possible fights, the battle is lost
                return -1;
            }
            //If there are attempts left, we will try going to the next remaining enemy. Automatically done with the for() (if there are any)
        }
        //If we reach this point, the battle is certainly won.
        return 1;
    }
    //If there are enemies left to fight, but we have no allies
    if (!remainingAttackers.empty() && remainingDefenders.empty()) {
        //OutputFTXUIText(settlementFightNoRemainingDefendersText, generalFightAttackerWinColor);
        Game::AddElementToFTXUIContainer(whereToDisplay,
                                         ftxui::paragraph(settlementFightNoRemainingDefendersText) | color(
                                             generalFightAttackerWinColor));
        //Battle is lost
        return -1;
    }
    //OutputFTXUIText(settlementFightNoRemainingAttackersText, generalFightDefenderWinColor);
    Game::AddElementToFTXUIContainer(whereToDisplay, ftxui::paragraph(settlementFightNoRemainingAttackersText) | color(generalFightDefenderWinColor));
    //Otherwise, the battle is won (no enemies left to fight)
    return 1;
}
*/

int Army::getTotalOverallPower() const { return totalOverallPower; }

//unsigned long Army::getGeneralCount() const { return assignedGenerals.size(); }

unsigned long Army::getUnitCount() const { return assignedUnits.size(); }

int Army::getCurrentActionPoints() const {
    return currentActionPoints;
}

int Army::useActionPoints(const int howMany) {
    if (currentActionPoints - howMany >= 0) {
        currentActionPoints -= howMany;
        return 1; //Action points depleted successfully
    }
    return -1; //Action points already depleted - shouldn't really happen
}

void Army::resetActionPoints() {
    currentActionPoints = defaultActionPoints;
}

//const std::vector<General> &Army::getAssignedGenerals() const { return assignedGenerals; }

const std::vector<std::shared_ptr<Unit> > &Army::getAssignedUnits() const { return assignedUnits; }

ftxui::Table Army::CreateArmyTable() const {
    std::vector<std::vector<std::string> > tableContent;

    tableContent.push_back(armyTableHeaders);

    int count = 0;
    for (const auto &unit: assignedUnits) {
        std::vector<std::string> tableRow = unit->getPrintableStats();
        tableRow.emplace(tableRow.begin(), std::to_string(count));
        tableContent.push_back(tableRow);

        count++;
    }

    //Display stuff
    using namespace ftxui;
    auto table = Table({tableContent});

    table.SelectAll().Border(LIGHT);

    //Separate all cells
    table.SelectAll().SeparatorVertical(LIGHT);

    //Make first row bold with a double border.
    table.SelectRow(0).Decorate(bold);
    table.SelectRow(0).SeparatorVertical(LIGHT);
    table.SelectRow(0).Border(DOUBLE);

    //Make the content a different color
    table.SelectRows(1, -1).DecorateCells(color(Color::LightSkyBlue1));

    return table;
}

void Army::DisplayArmy() const {
    //Legacy display
    using namespace ftxui;

    auto table = CreateArmyTable();

    auto document = table.Render();
    auto screen =
            Screen::Create(Dimension::Fit(document, /*extend_beyond_screen=*/true));
    Render(screen, document);
    screen.Print();
    std::cout << std::endl;
}

ftxui::Element Army::FTXUIDisplayArmy() const {
    using namespace ftxui;

    Table table = CreateArmyTable();

    auto document = table.Render();

    return document;
}

Army::Army(const Army &other) : defaultActionPoints(other.defaultActionPoints),
                                currentActionPoints(other.currentActionPoints),
                                totalOverallPower(other.totalOverallPower),
                                isStationed(other.isStationed) {
    for (const auto &unit: other.assignedUnits) {
        assignedUnits.push_back(unit->clone());
    }
}

Army &Army::operator=(Army other) {
    swap(*this, other);
    return *this;
}

void swap(Army &first, Army &second) {
    using std::swap;
    swap(first.assignedUnits, second.assignedUnits);
    swap(first.currentActionPoints, second.currentActionPoints);
    swap(first.totalOverallPower, second.totalOverallPower);
    swap(first.isStationed, second.isStationed);
}
