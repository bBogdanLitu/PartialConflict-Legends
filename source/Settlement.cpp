#include "../header/Settlement.h"

#include <ftxui/component/event.hpp>
#include "../header/Game.h"
#include <ftxui/component/screen_interactive.hpp>


Settlement::Settlement(const Garrison &garrison_, std::string name_, int owner_,
                       int index_, long int income_) : stationedGarrison(garrison_),
                                                       name(std::move(name_)),
                                                       owner(owner_),
                                                       index(index_),
                                                       income(income_) {
}

void Settlement::StationArmy(const std::shared_ptr<Army> &army) {
    stationedArmy = army;
}

void Settlement::StationTemporaryArmy(const std::shared_ptr<Army> &army) {
    temporaryArmy = army;
}

//An army is sent towards a final destination. Moving armies is done through the settlements they are stationed in.
//As such, it is moved from settlement to settlement, while keeping in mind what route to take.
//(doesn't station in a control point, it only checks if it has enough action points to pass)
void Settlement::SendArmy(const std::shared_ptr<Army> &travellingArmy, std::vector<int> targetIndexes, Enemy *sender,
                          const ftxui::Component &gameWindow) {
    //We are certain that this current settlement neighbours the target.
    int targetIndex = targetIndexes[targetIndexes.size() - 1];
    int cost = 9999; //so it doesn't cry

    //We temporarily store this army in this settlement
    StationTemporaryArmy(travellingArmy);

    //Searching for the control point that is either owned by or connected to the target settlement
    for (const auto &controlPoint: ControlPoints) {
        if (controlPoint.getIndexOfConnectedSettlement() == targetIndex || controlPoint.getIndexOfOwnerSettlement() ==
            targetIndex) {
            cost = controlPoint.getTravelCost();
            break; //found it
        }
    }
    if (targetIndexes.size() == 1) {
        //If we reached the last settlement before the target, we will try to attack the settlement
        // after checking the actionPoints;
        if (cost <= travellingArmy->getCurrentActionPoints()) {
            DetachTemporaryArmy();
            travellingArmy->useActionPoints(cost);
            //attack
            for (const auto &neighbourWeak: Neighbours) {
                if (auto neighbour = neighbourWeak.lock()) {
                    if (neighbour->getIndex() == targetIndex) {
                        AttackAndAnalyzeResult(neighbour, travellingArmy, sender, gameWindow);
                    }
                }
            }
        }
    } else {
        targetIndexes.pop_back(); //remove the one we will send it to right now
        //send it forward to the next Settlement
        for (const auto &neighbourWeak: Neighbours) {
            if (auto neighbour = neighbourWeak.lock()) {
                if (neighbour->getIndex() == targetIndex) {
                    if (cost <= travellingArmy->getCurrentActionPoints()) {
                        DetachTemporaryArmy();
                        //we no longer need to store it here, it will pass to the next settlement.
                        neighbour->SendArmy(travellingArmy, targetIndexes, sender, gameWindow);
                        break; //found
                    } else {
                        //The army cannot pass further. Should try again next turn... NIGHTMARE TO IMPLEMENT
                        break;
                    }
                }
            }
        }
    }
}

void Settlement::AttackAndAnalyzeResult(const std::shared_ptr<Settlement> &neighbour,
                                        const std::shared_ptr<Army> &travellingArmy, Enemy *sender,
                                        const ftxui::Component &gameWindow) {
    Game::AddElementToFTXUIContainer(
        gameWindow,
        ftxui::paragraph("The battle of " + neighbour->name + " will now commence") | ftxui::color(
            importantGameInformationColor));
    Game::AddNewLineToFTXUIContainer(gameWindow);
    int result = neighbour->Besieged(*travellingArmy, gameWindow);

    switch (result) {
        case 1: {
            travellingArmy->Disband();
            Game::AddElementToFTXUIContainer(gameWindow, ftxui::paragraph("Won"));
            break;
        }
        case -1: {
            neighbour->ChangeOwnership(sender);
            neighbour->StationArmy(travellingArmy);
            Game::AddElementToFTXUIContainer(
                gameWindow,
                ftxui::paragraph("Lost " + neighbour->name) | ftxui::color(
                    importantGameInformationColor));
            break;
        }
        default: {
            Game::AddElementToFTXUIContainer(gameWindow, ftxui::paragraph("asdsa32113sdasdasda"));
        }

        break;
    }
}


void Settlement::DetachArmy() {
    stationedArmy.reset();
}

void Settlement::DetachTemporaryArmy() {
    temporaryArmy.reset();
}

//will check if this settlement neighbours a settlement with a specific owner and will return -1 if not,
// or the searched index if it does.
int Settlement::CheckNeighboursOwner(int wantedOwnerIndex) const {
    for (const auto &neighbourWeak: Neighbours) {
        if (auto neighbour = neighbourWeak.lock()) {
            if (neighbour->getOwner() == wantedOwnerIndex) {
                return neighbour->getIndex();
            }
        }
    }
    return -1;
}

void Settlement::AddControlPoint(const ControlPoint &controlPoint) {
    ControlPoints.push_back(controlPoint);
}

void Settlement::AddNeighbour(const std::shared_ptr<Settlement> &neighbour) {
    Neighbours.push_back(neighbour);
}


void Settlement::AddUnitToArmy(const std::shared_ptr<Unit> &unit) {
    stationedArmy.value()->AddUnit(unit);
}

int Settlement::getOwner() const {
    return owner;
}

long int Settlement::getIncome() const {
    return income;
}

int Settlement::getIndex() const {
    return index;
}

std::optional<std::shared_ptr<Army> > Settlement::getStationedArmy() const {
    if (stationedArmy.has_value()) {
        return stationedArmy.value();
    }
    return std::nullopt;
}

/*
 *DISCLAIMER (in ro): Stiu ca este o functie gigantica, dar asta se datoreaza afisarii cu elemente ftxui.
 *Daca as face-o bucatele cred ca ar deveni si mai greu de inteles :{
*/
//If there is a stationedArmy, there will be a combat prompt to the player.
//If not, then the player will only get the notification of the outcome.
int Settlement::Besieged(const Army &attackingArmy, const ftxui::Component &gameWindow) const {
    //1 = victory , -1 = defeat, 0 = nothing happened (?)
    int result = 0;
    if (stationedArmy.has_value()) {
        //fight with army+garrison
        std::vector<unsigned long> battleOrder = {0};
        unsigned long neededInputs = stationedArmy.value()->getUnitCount();
        std::string input1, input2, input3;
        using namespace ftxui;

        //a new temporary screen
        auto screen = ScreenInteractive::FitComponent();

        auto doneStyle = ButtonOption::Animated(Color::Default, beautifulBlue,
                                                Color::Default, Color::White);

        //button functions
        auto onDoneButtonClick = [&] {
            //I only check if they are empty. Any other verification (whether they are digits) will be done elsewhere.
            battleOrder.clear();
            if (neededInputs > 2) {
                if (!input3.empty()) {
                    battleOrder.emplace(battleOrder.begin(), std::stoul(input3));
                }
            }
            if (neededInputs > 1) {
                if (!input2.empty()) {
                    battleOrder.emplace(battleOrder.begin(), std::stoul(input2));
                }
            }
            if (!input1.empty()) {
                battleOrder.emplace(battleOrder.begin(), std::stoul(input1));
                screen.Exit();
            }
        };

        //input styles
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

        Component battleInputContainer = Container::Vertical({});
        Component inputReaderContainer = Container::Vertical({});
        Component battleInformationContainer = Container::Vertical({});

        Component inputComp1 = ftxui::Input(&input1, "First enemy to battle with your (index):", inputOption),
                inputComp2 = ftxui::Input(&input2, "Second enemy to battle with your (index):", inputOption),
                inputComp3 = ftxui::Input(&input3, "Third enemy to battle with your (index):", inputOption);
        Component doneButton = Button("Done", onDoneButtonClick, doneStyle);

        //information that will be shown after choosing the battle order.
        Game::AddElementToFTXUIContainer(gameWindow, stationedArmy.value()->FTXUIDisplayArmy());
        Game::AddNewLineToFTXUIContainer(gameWindow);
        Game::AddElementToFTXUIContainer(gameWindow, paragraph("VERSUS") | color(beautifulOrange));
        Game::AddNewLineToFTXUIContainer(gameWindow);
        Game::AddElementToFTXUIContainer(gameWindow, attackingArmy.FTXUIDisplayArmy());
        Game::AddNewLineToFTXUIContainer(gameWindow);
        Game::AddElementToFTXUIContainer(gameWindow, paragraph("How it went: ") | color(gameAnnouncementsColor));

        //information to be shown on the temporary screen
        Game::AddElementToFTXUIContainer(battleInformationContainer,
                                         paragraph(
                                             "Your settlement " + name +
                                             " is being attacked! You must choose the battle order.")
                                         | color(importantGameInformationColor));
        Game::AddNewLineToFTXUIContainer(battleInformationContainer);
        Game::AddElementToFTXUIContainer(battleInformationContainer, paragraph("Your army:"));
        Game::AddElementToFTXUIContainer(battleInformationContainer, stationedArmy.value()->FTXUIDisplayArmy());
        Game::AddNewLineToFTXUIContainer(battleInformationContainer);
        Game::AddElementToFTXUIContainer(battleInformationContainer, paragraph("Enemy army:"));
        Game::AddElementToFTXUIContainer(battleInformationContainer, attackingArmy.FTXUIDisplayArmy());
        Game::AddNewLineToFTXUIContainer(battleInformationContainer);
        Game::AddElementToFTXUIContainer(battleInformationContainer, paragraph("Choose order:"));

        inputComp1 |= CatchEvent([&](const Event &event) {
            if (event.is_character() && !std::isdigit(event.character()[0])) {
                return true; //it's not a digit, catch it and prevent it from modifying tempInput
            }
            //check if the digit is any other than 0, 1, 2, and catch it.
            if (event.is_character() && event.character()[0] > '2') {
                return true;
            }
            return false; //it's a digit that we want
        });
        inputComp2 |= CatchEvent([&](const Event &event) {
            if (event.is_character() && !std::isdigit(event.character()[0])) {
                return true; //it's not a digit, catch it and prevent it from modifying tempInput
            }
            //check if the digit is any other than 0, 1, 2, and catch it.
            if (event.is_character() && event.character()[0] > '2') {
                return true;
            }
            return false; //it's a digit that we want
        });
        inputComp3 |= CatchEvent([&](const Event &event) {
            if (event.is_character() && !std::isdigit(event.character()[0])) {
                return true; //it's not a digit, catch it and prevent it from modifying tempInput
            }
            //check if the digit is any other than 0, 1, 2, and catch it.
            if (event.is_character() && event.character()[0] > '2') {
                return true;
            }
            return false; //it's a digit that we want
        });

        inputReaderContainer->Add(inputComp1);
        if (neededInputs > 1) {
            inputReaderContainer->Add(inputComp2);
        }
        if (neededInputs > 2) {
            inputReaderContainer->Add(inputComp3);
        }
        inputReaderContainer->Add(doneButton);
        battleInputContainer->Add(battleInformationContainer);
        battleInputContainer->Add(inputReaderContainer);

        auto renderer = Renderer(battleInputContainer, [&] {
            return vbox({
                battleInformationContainer->Render(),
                //now everything in inputReaderContainer
                inputComp1->Render() | size(HEIGHT, GREATER_THAN, Terminal::Size().dimy / 100.0f * 5)
                | size(WIDTH, GREATER_THAN, Terminal::Size().dimx / 100.0f * 100),
                inputComp2->Render() | size(HEIGHT, GREATER_THAN, Terminal::Size().dimy / 100.0f * 5),
                inputComp3->Render() | size(HEIGHT, GREATER_THAN, Terminal::Size().dimy / 100.0f * 5),
                separator(),
                doneButton->Render() | size(HEIGHT, GREATER_THAN, Terminal::Size().dimy / 100.0f * 5),
            });
        });

        //only makes sense to let the player choose if he has more than 1 general in his army
        if (neededInputs > 1) {
            screen.Loop(renderer);
        }

        //To prevent assigning one general to fight multiple enemies (at once)
        //If k was equal once, it will be equal the second time (like, for real),
        //so it is wrong to restart it from 0 every time it loops.
        unsigned long k = 0;
        while (battleOrder.size() < neededInputs) {
            battleOrder.emplace_back(2);
        }
        for (unsigned long i = 0; i < battleOrder.size(); i++) {
            if (battleOrder[i] >= neededInputs) {
                battleOrder[i] = neededInputs - 1; //capping to the last possible one
            }
        }
        //We search for the first unassigned general and make it assigned instead.
        for (unsigned long i = 1; i < battleOrder.size(); i++) {
            for (unsigned long j = 0; j < i; j++) {
                while (battleOrder[i] == battleOrder[j] && k < neededInputs) {
                    battleOrder[i] = k;
                    k++;
                }
            }
        }

        /*Will keep this temporarily if the method above ever backfires
        //temp
        auto battleOrderP = battleOrder;
        battleOrderP.clear();
        for (unsigned long i = 0; i < neededInputs; i++) {
            battleOrderP.emplace_back(i);
        }
        */
        result = stationedArmy.value()->Attacked(attackingArmy, stationedGarrison.GetOverallPower(), battleOrder,
                                                 gameWindow);
    } else {
        //fight with garrison
        result = stationedGarrison.DirectlyAttacked(attackingArmy);
    }
    return result;
}

void Settlement::ChangeOwnership(Enemy *newOwner) {
    stationedArmy.reset();
    owner = newOwner->getIndex();
    if (auto selfPtr = weakSelfPtr.lock()) {
        newOwner->ModifySettlementOwnership(selfPtr);
    }
}

void Settlement::GiveToPlayer(const ftxui::Component &gameWindow) {
    owner = 0;
    Game::AddElementToFTXUIContainer(
        gameWindow,
        ftxui::paragraph("You have conquered the settlement named " + name) | ftxui::color(gameAnnouncementsColor));
}

void Settlement::setSelfPtr(const std::shared_ptr<Settlement> &settlement) {
    weakSelfPtr = settlement;
}

ftxui::Table Settlement::CreateSettlementsTable() const {
    std::vector<std::vector<std::string> > tableContent;
    std::vector<std::string> tableRow;
    std::string neighboursConverted;
    //Headers for the FTXUI table
    tableContent.push_back(settlementTableHeaders);

    //Populating the (only) row
    std::string indexConverted = std::to_string(index);
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
    for (unsigned long i = 0; i < Neighbours.size(); i++) {
        neighboursConverted += std::to_string(Neighbours[i].lock()->getIndex()) + " ";
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

void Settlement::DisplaySettlement() const {
    //Legacy display stuff
    using namespace ftxui;

    Table table = CreateSettlementsTable();

    auto document = table.Render();
    auto screen =
            Screen::Create(Dimension::Fit(document, /*extend_beyond_screen=*/true));
    Render(screen, document);
    screen.Print();
    std::cout << std::endl;
}

ftxui::Element Settlement::FTXUIDisplaySettlement() const {
    using namespace ftxui;

    Table table = CreateSettlementsTable();

    auto document = table.Render();

    return document;
}

Settlement::Settlement(const Settlement &other) : stationedArmy(other.stationedArmy),
                                                  temporaryArmy(other.temporaryArmy),
                                                  stationedGarrison(other.stationedGarrison),
                                                  ControlPoints(other.ControlPoints),
                                                  name(other.name),
                                                  owner(other.owner),
                                                  index(other.index),
                                                  income(other.income) {
    for (const auto &neighbour: other.Neighbours) {
        Neighbours.push_back(neighbour);
    }
}

Settlement &Settlement::operator=(Settlement other) {
    swap(*this, other);
    return *this;
}

Settlement::~Settlement() {
    Neighbours.clear();
    ControlPoints.clear();

    stationedArmy.reset();
    temporaryArmy.reset();
}


void swap(Settlement &first, Settlement &second) {
    std::swap(first.stationedGarrison, second.stationedGarrison);
    std::swap(first.ControlPoints, second.ControlPoints);
    std::swap(first.name, second.name);
    std::swap(first.owner, second.owner);
    std::swap(first.index, second.index);
    std::swap(first.income, second.income);
    std::swap(first.Neighbours, second.Neighbours);
    std::swap(first.temporaryArmy, second.temporaryArmy);
    std::swap(first.stationedArmy, second.stationedArmy);
}
