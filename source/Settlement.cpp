#include "../header/Settlement.h"

Settlement::Settlement(const Garrison &garrison_, std::string name_, int owner_) : stationedGarrison(garrison_),
    name(std::move(name_)),
    owner(owner_) {
}

void Settlement::StationArmy(const Army &army) {
    stationedArmy = army;
}

void Settlement::AddControlPoint(const ControlPoint &controlPoint) {
    ControlPoints.push_back(controlPoint);
}

void Settlement::AddNeighbour(int neighbourIndex) {
    neighbours.push_back(neighbourIndex);
}

void Settlement::AddUnitToArmy(const std::shared_ptr<Unit> &unit) {
    stationedArmy.value().AddUnit(unit);
}

void Settlement::Besieged(const Army &attackingArmy) const {
    //If there is a stationedArmy, there will be a combat prompt to the player.
    //If not, then the player will only get the notification of the outcome.
    int result;
    if (stationedArmy.has_value()) {
        OutputFTXUIText(settlementStationedArmyText, allyRelatedTextColor);
        stationedArmy.value().DisplayArmy();
        OutputFTXUIText(chooseBattleOrderText, importantGameInformationColor);

        std::vector<unsigned long> battleOrder;
        //Choosing the order until it is useless to do so.
        for (unsigned long i = 0;
             i < stationedArmy.value().getUnitCount() && i < attackingArmy.getUnitCount();
             i++) {
            unsigned long a;
            OutputFTXUIText("Enemy " + std::to_string(i) + " to fight with your: ", userInputExpectedColor);
            std::cin >> a;
            //Sanitizing user input
            sanitizeInputMore(a);
            if (a >= stationedArmy.value().getUnitCount()) {
                a = stationedArmy.value().getUnitCount() - 1; //capping to the last possible one
            }
            //To prevent assigning one general to fight multiple enemies (at once)
            //If k was equal once, it will be equal the second time (like, for real),
            //so it is wrong to restart it from 0 every time it loops.
            unsigned long k = 0;
            for (const unsigned long j: battleOrder) {
                //We search for the first unassigned general and make it assigned instead.
                while (j == a && k <= armyGeneralsMaximumIndex) {
                    a = k;
                    k++;
                }
            }
            battleOrder.push_back(a);
        }

        result = stationedArmy.value().Attacked(attackingArmy, stationedGarrison.GetOverallPower(), battleOrder);
    } else {
        std::cout << settlementNoStationedArmyText;
        result = stationedGarrison.DirectlyAttacked(attackingArmy);
    }
    switch (result) {
        case 1: {
            OutputFTXUIText(this->name + settlementBesiegeFailedText, importantGameInformationColor);
            break;
        }
        case -1: {
            OutputFTXUIText(this->name + settlementBesiegeSuccessText, importantGameInformationColor);
            break;
        }
        default: {
            std::cerr << "Undefined behaviour detected!" << "\n";
        }
    }
}

void Settlement::DisplaySettlement(unsigned long indexInTheSettlementVector) const {
    std::vector<std::vector<std::string> > tableContent;
    std::vector<std::string> tableRow;
    std::string neighboursConverted;
    //Headers for the FTXUI table
    tableContent.push_back(settlementTableHeaders);

    //Populating the (only) row
    std::string indexConverted = std::to_string(indexInTheSettlementVector);
    tableRow.push_back(indexConverted);
    tableRow.push_back(name);
    if (owner == 0) {
        tableRow.push_back("You");
    } else {
        tableRow.push_back(std::to_string(owner));
    }
    tableRow.push_back(std::to_string(stationedGarrison.GetOverallPower()));
    if (stationedArmy != std::nullopt) {
        tableRow.push_back("Yes");
    } else {
        tableRow.push_back("No");
    }
    tableRow.push_back(std::to_string(ControlPoints.size()));
    for (unsigned long i = 0; i < neighbours.size(); i++) {
        neighboursConverted += std::to_string(neighbours[i]) + " ";
    }
    tableRow.push_back(neighboursConverted);

    //Push the row to display it
    tableContent.push_back(tableRow);

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
    table.SelectRow(1).DecorateCells(color(Color::GreenYellow));

    auto document = table.Render();
    auto screen =
            Screen::Create(Dimension::Fit(document, /*extend_beyond_screen=*/true));
    Render(screen, document);
    screen.Print();
    std::cout << std::endl;
}
