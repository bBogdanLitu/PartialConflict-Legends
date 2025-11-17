#include "../header/Settlement.h"
#include "../header/Game.h"
#include "ftxui/component/event.hpp"

Settlement::Settlement(const Garrison &garrison_, std::string name_, int owner_,
                       long int income_) : stationedGarrison(garrison_),
                                           name(std::move(name_)),
                                           owner(owner_),
                                           income(income_) {
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

int Settlement::getOwner() const {
    return owner;
}

long int Settlement::getIncome() const {
    return income;
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

//If there is a stationedArmy, there will be a combat prompt to the player.
//If not, then the player will only get the notification of the outcome.
void Settlement::FTXUIBesieged(const Army &attackingArmy, const ftxui::Component &whereToDisplay) const {
    int result;
    std::vector<std::string> boInputStrings(3, "");
    std::vector<unsigned long> battleOrder, availableAllyIndexes;
    using namespace ftxui; //it's a pain otherwise
    //STYLE
    //for input
    InputOption inputOption = InputOption::Spacious();
    inputOption.transform = [](InputState state) {
        state.element |= color(userInputExpectedColor);
        if (state.focused) {
            state.element |= bgcolor(Color::Default);
        } else if (state.hovered) {
            state.element |= bgcolor(Color::Grey15);
        } else {
            state.element |= bgcolor(Color::Grey27);
        }
        return state.element;
    };

    //for button
    auto confirmBOButtonStyle = ButtonOption::Animated(Color::Default, Color::GrayDark,
                                                       Color::Default, Color::White);

    //FUNCTIONS
    //for buttons
    auto onConfirmBOButtonClick = [&] {
        for (auto i: boInputStrings) {
            if (i.empty()) {
                i = "0";
                //if the user didn't write anything in an input, I will assign 0 to it and let the following code do its magic and assign it
            }
            unsigned long value = std::stoul(i);
            if (value >= stationedArmy.value().getUnitCount()) {
                value = stationedArmy.value().getUnitCount() - 1; //capping to the last possible one
            }
            //To prevent assigning one general to fight multiple enemies (at once)
            //If k was equal once, it will be equal the second time (like, for real),
            //so it is wrong to restart it from 0 every time it loops.
            unsigned long k = 0;
            for (const unsigned long j: battleOrder) {
                //We search for the first unassigned general and make it assigned instead.
                while (j == value && k <= armyGeneralsMaximumIndex) {
                    value = k;
                    k++;
                }
            }
            battleOrder.push_back(value);
        }
        //and I can finally get the result of the fight (after fixing the display stuff in Army)
        result = stationedArmy.value().FTXUIAttacked(attackingArmy, stationedGarrison.GetOverallPower(), battleOrder,
                                                     whereToDisplay);
    };


    if (stationedArmy.has_value()) {
        Game::AddElementToFTXUIContainer(whereToDisplay,
                                         paragraph(settlementStationedArmyText) | color(allyRelatedTextColor));
        Game::AddElementToFTXUIContainer(whereToDisplay, stationedArmy.value().FTXUIDisplayArmy());
        Game::AddElementToFTXUIContainer(whereToDisplay,
                                         paragraph(chooseBattleOrderText) | color(allyRelatedTextColor));


        for (unsigned long i = 0; i < stationedArmy.value().getUnitCount(); i++) {
            availableAllyIndexes.push_back(i);
        }

        //Choosing the order until it is useless to do so.
        for (unsigned long i = 0;
             i < stationedArmy.value().getUnitCount() && i < attackingArmy.getUnitCount();
             i++) {
            //to get the battle order I will use an input
            Component boInput = Input(boInputStrings[i],
                                      "Enemy with index " + std::to_string(i) + " to fight with your: ", inputOption);

            boInput |= CatchEvent([&](const Event &event) {
                if (event.is_character() && !std::isdigit(event.character()[0])) {
                    return true; //it isn't a digit and shouldn't modify the input
                }
                if (event == Event::Return) {
                    return true; //the input doesn't require more than one line
                }
                return false;
            });
            Game::AddComponentToFTXUIContainer(whereToDisplay, boInput);
            Game::AddElementToFTXUIContainer(whereToDisplay, paragraph(" ")); //newline
        }

        //to validate the results, I will add a button that should be pressed when the user has entered everything
        auto confirmBOButton = Button("Confirm order", onConfirmBOButtonClick, confirmBOButtonStyle);
        Game::AddComponentToFTXUIContainer(whereToDisplay, confirmBOButton);
    } else {
        //std::cout << settlementNoStationedArmyText;
        Game::AddElementToFTXUIContainer(whereToDisplay,
                                         paragraph(settlementNoStationedArmyText) | color(allyRelatedTextColor));
        result = stationedGarrison.DirectlyAttacked(attackingArmy);
    }
    switch (result) {
        case 1: {
            Game::AddElementToFTXUIContainer(whereToDisplay,
                                             paragraph(this->name + settlementBesiegeFailedText) | color(
                                                 importantGameInformationColor));
            break;
        }
        case -1: {
            Game::AddElementToFTXUIContainer(whereToDisplay,
                                             paragraph(this->name + settlementBesiegeSuccessText) | color(
                                                 importantGameInformationColor));
            break;
        }
        default: {
            std::cerr << "Undefined behaviour detected!" << "\n";
        }
    }
}

ftxui::Table Settlement::CreateSettlementsTable(unsigned long indexInTheSettlementVector) const {
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
    using namespace ftxui;

    tableContent.push_back(tableRow);

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

    return table;
}

void Settlement::DisplaySettlement(unsigned long indexInTheSettlementVector) const {
    //Legacy display stuff
    using namespace ftxui;

    Table table = CreateSettlementsTable(indexInTheSettlementVector);

    auto document = table.Render();
    auto screen =
            Screen::Create(Dimension::Fit(document, /*extend_beyond_screen=*/true));
    Render(screen, document);
    screen.Print();
    std::cout << std::endl;
}

ftxui::Element Settlement::FTXUIDisplaySettlement(unsigned long indexInTheSettlementVector) const {
    using namespace ftxui;

    Table table = CreateSettlementsTable(indexInTheSettlementVector);

    auto document = table.Render();

    return document;
}
