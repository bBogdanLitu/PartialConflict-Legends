#include "../header/Army.h"
#include "../header/Game.h"

int Army::TotalOverallPowerCalculation(const std::shared_ptr<Unit> &unit) {
    totalOverallPower += unit->getOverallPower();
    return totalOverallPower;
}

Army::Army(const std::shared_ptr<Unit> &unit) {
    if (AssignedUnits.size() < 3) {
        AssignedUnits.push_back(unit);
        TotalOverallPowerCalculation(unit);
    }
}

void Army::AddUnit(const std::shared_ptr<Unit> &unit) {
    if (AssignedUnits.size() < 3) {
        AssignedUnits.push_back(unit);
    }
}

void Army::RemoveUnit(const unsigned long &index) {
    AssignedUnits.erase(AssignedUnits.begin() + index);
    if (AssignedUnits.size() == 0) {
        Disband();
    }
}

//no idea how to split this monster into 2 or more
int Army::Attacked(const Army &attackingArmy, const int overallBoost,
                   const std::vector<unsigned long> &battleOrder, const ftxui::Component &gameWindow) const {
    int currentAttacker = 0;
    std::vector<int> remainingAttackers, remainingDefenders, surplusDefenders;
    //We assume that every defender and attacker can be surplus (apriori)
    for (int i = 0; i < static_cast<int>(this->getUnitCount()); i++) {
        surplusDefenders.push_back(i);
    }

    for (const unsigned long currentDefender: battleOrder) {
        //We mark the chosen defender as non-surplus
        surplusDefenders[currentDefender] = -1;
        int fightResult = this->getAssignedUnits()[currentDefender]->
                FightWith(*attackingArmy.getAssignedUnits()[currentAttacker], overallBoost, gameWindow);
        //defender combat is boosted by the garrison
        switch (fightResult) {
            case 0: {
                Game::AddElementToFTXUIContainer(
                    gameWindow,
                    ftxui::paragraph("Fight won by the attacker.") | ftxui::color(generalFightAttackerWinColor));
                Game::AddNewLineToFTXUIContainer(gameWindow);
                //Save the index of the undefeated attacker
                remainingAttackers.push_back(currentAttacker);
                break;
            }
            case 1: {
                Game::AddElementToFTXUIContainer(
                    gameWindow,
                    ftxui::paragraph("Fight won by the defender.") | ftxui::color(generalFightDefenderWinColor));
                Game::AddNewLineToFTXUIContainer(gameWindow);
                //Save the index of the undefeated defender
                remainingDefenders.push_back(static_cast<int>(currentDefender));
                break;
            }
            default: {
                std::cout << "Unhandled\n";
            }
        }
        currentAttacker++;
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
        Game::AddElementToFTXUIContainer(
            gameWindow, ftxui::paragraph(settlementFightRemainingAttackersText) | ftxui::color(gameAnnouncementsColor));

        //We still have to defeat the undefeated enemy using remaining generals
        unsigned long attempts = 0;
        int fightResult = 0;
        for (const int remainingAttacker: remainingAttackers) {
            //We will attempt to defeat all remaining enemies using our allies.
            //We stop when either they are all defeated or we don't have any allies left.
            do {
                fightResult = this->getAssignedUnits()[remainingDefenders[attempts]]->FightWith(
                    *attackingArmy.getAssignedUnits()[remainingAttacker], overallBoost, gameWindow);
                if (fightResult == 0) {
                    Game::AddElementToFTXUIContainer(
                        gameWindow,
                        ftxui::paragraph("Fight won by the attacker.") | ftxui::color(generalFightAttackerWinColor));
                    attempts++; //only try going to the next remaining ally if the current loses a battle.
                } else {
                    Game::AddElementToFTXUIContainer(
                        gameWindow,
                        ftxui::paragraph("Fight won by the defender.") | ftxui::color(generalFightDefenderWinColor));
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
        Game::AddElementToFTXUIContainer(
            gameWindow,
            ftxui::paragraph(settlementFightNoRemainingDefendersText) | ftxui::color(gameAnnouncementsColor));
        //Battle is lost
        return -1;
    }

    Game::AddElementToFTXUIContainer(
        gameWindow, ftxui::paragraph(settlementFightNoRemainingAttackersText) | ftxui::color(gameAnnouncementsColor));
    //Otherwise, the battle is won (no enemies left to fight)
    return 1;
}

int Army::getTotalOverallPower() const { return totalOverallPower; }

//unsigned long Army::getGeneralCount() const { return assignedGenerals.size(); }

unsigned long Army::getUnitCount() const { return AssignedUnits.size(); }

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

const std::vector<std::shared_ptr<Unit> > &Army::getAssignedUnits() const { return AssignedUnits; }

ftxui::Table Army::CreateArmyTable() const {
    std::vector<std::vector<std::string> > tableContent;

    tableContent.push_back(armyTableHeaders);

    int count = 0;
    for (const auto &unit: AssignedUnits) {
        std::vector<std::string> tableRow = unit->getPrintableStats();
        //add the action points
        tableRow.emplace(tableRow.begin(), std::to_string(defaultActionPoints));
        tableRow.emplace(tableRow.begin(), std::to_string(currentActionPoints));
        //add the count
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

void Army::Disband() {
    AssignedUnits.clear();
    isStationed = false;
}

Army::Army(const Army &other) : defaultActionPoints(other.defaultActionPoints),
                                currentActionPoints(other.currentActionPoints),
                                totalOverallPower(other.totalOverallPower),
                                isStationed(other.isStationed) {
    for (const auto &unit: other.AssignedUnits) {
        AssignedUnits.push_back(unit->clone());
    }
}

Army &Army::operator=(Army other) {
    swap(*this, other);
    return *this;
}

void swap(Army &first, Army &second) {
    using std::swap;
    swap(first.AssignedUnits, second.AssignedUnits);
    swap(first.currentActionPoints, second.currentActionPoints);
    swap(first.totalOverallPower, second.totalOverallPower);
    swap(first.isStationed, second.isStationed);
}
