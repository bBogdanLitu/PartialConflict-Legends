#ifndef Settlement_H
#define Settlement_H
#include <optional>
#include <nlohmann/detail/string_utils.hpp>
#include <ftxui/dom/table.hpp>

#include "ControlPoint.h"
#include "Garrison.h"

//The objective of the game, conquering them leads to victory. Has an Army and/or a (weak) Garrison stationed.
//There are Control Points assigned to a Settlement
class Settlement {
private:
    std::optional<Army> stationedArmy;
    Garrison stationedGarrison;
    std::vector<ControlPoint> controlPoints;
    std::string name;
    int owner; //0 = player, others are enemies or contenders
    std::vector<int> neighbours; //the index of neighbouring settlements (in the vector)

public:
    Settlement(const Garrison &garrison_, const std::vector<ControlPoint> &controlPoints_, std::string name_,
               int owner_, std::vector<int> neighbours_) : stationedGarrison(garrison_), controlPoints(controlPoints_),
                                                           name(std::move(name_)),
                                                           owner(owner_), neighbours(std::move(neighbours_)) {
    }

    void StationArmy(const Army& army) {
        stationedArmy = army;
    }

    //Because Armies can only be modified when they are in a Settlement! (or directly only for test purposes)
    void AddGeneralToArmy(const General &general) {
        stationedArmy.value().AddGeneral(general);
    }

    void Besieged(const Army &attackingArmy) const;

    /*[[nodiscard]] std::optional<Army> getStationedArmy() const {
        if (stationedArmy.has_value()) {
            return stationedArmy.value();
        }
        return std::nullopt;
    }*/

    /*void StationArmyInControlPoint(const Army& army, const int index) {
        controlPoints[index].StationArmy(army);
    }*/

    //There can only be one Army stationed at a time, so I simply remove it.
    /*void MoveArmy(const Army& army) {

        stationedArmy = std::nullopt;
    }*/

    void DisplaySettlement() const;

    friend std::ostream& operator<<(std::ostream& os, const Settlement& settlement) {
        int k = 0;
        os << "Settlement name: " << settlement.name << "\n";
        if (settlement.owner == 0) {
            os << "Owned by the player\n";
        }
        os << settlement.stationedGarrison;
        os << "List of attached control points:\n";
        for (const auto &i: settlement.controlPoints) {
            os << k << ".\n" << i << "\n";
            k++;
        }
        os << "\n";
        if (settlement.stationedArmy.has_value()) {
            os << "And this is the stationed army. " << settlement.stationedArmy.value() << "\n";
        }
        return os;
    }
};

inline void Settlement::Besieged(const Army &attackingArmy) const {
    //If there is a stationedArmy, there will be a combat prompt to the player.
    //If not, then the player will only get the notification of the outcome.
    int result;
    if (stationedArmy.has_value()) {
        //std::cout << settlementStationedArmyText;
        OutputFTXUIText(settlementStationedArmyText, allyRelatedTextColor);
        //std::cout << stationedArmy.value();
        stationedArmy.value().DisplayArmy();
        //std::cout << chooseBattleOrderText;
        OutputFTXUIText(chooseBattleOrderText, importantGameInformationColor);

        std::vector<unsigned long> battleOrder;
        //Choosing the order until it is useless to do so.
        for (unsigned long i = 0;
             i < stationedArmy.value().getGeneralCount() && i < attackingArmy.getGeneralCount();
             i++) {
            unsigned long a;
            //std::cout << "Enemy " << i << " to fight with your: ";
            OutputFTXUIText("Enemy " + std::to_string(i) + " to fight with your: ", userInputExpectedColor);
            std::cin >> a;
            //Sanitizing user input
            sanitizeInputMore(a);
            if (a > stationedArmy.value().getGeneralCount()) {
                a = stationedArmy.value().getGeneralCount() - 1; //capping to the last possible one
            }
            //To prevent assigning one general to fight multiple enemies (at once)
            for (const unsigned long j: battleOrder) {
                //We search for the first unassigned general and make it assigned instead.
                unsigned long k = 0;
                while (j == a && k <= armyGeneralsMaximumIndex) {
                    a = ++k;
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
            //std::cout << this->name << settlementBesiegeFailedText << "\n";
            OutputFTXUIText(this->name + settlementBesiegeFailedText, importantGameInformationColor);
            break;
        }
        case -1: {
            //std::cout << this->name << settlementBesiegeSuccessText << "\n";
            OutputFTXUIText(this->name + settlementBesiegeSuccessText, importantGameInformationColor);
            break;
        }
        default: {
            std::cerr << "Undefined behaviour detected!" << "\n";
        }
    }
}

inline void Settlement::DisplaySettlement() const {
    std::vector<std::vector<std::string> > tableContent;
    std::vector<std::string> tableRow;
    std::string neighboursConverted;
    //Headers for the FTXUI table
    tableContent.push_back(settlementTableHeaders);

    //Populating the (only) row
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
    tableRow.push_back(std::to_string(controlPoints.size()));
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

#endif
