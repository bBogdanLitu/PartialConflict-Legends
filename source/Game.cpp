#include <iostream>
#include <fstream>
#include <vector>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/dom/table.hpp>
#include <nlohmann/json.hpp>
#include "../header/Game.h"
#include "../header/Settlement.h"
#include "../header/Functions.h"
#include "../header/Captain.h"
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ctime>

#include "../header/Except.h"
#include "../header/LocalLeader.h"

using namespace ftxui;

//helper functions for ftxui
ButtonOption ButtonStyleCenterText(Color foreground, Color foregroundActive, Color background, Color backgroundActive) {
    auto option = ButtonOption::Animated(background, foreground, backgroundActive, foregroundActive);
    option.transform = [](const EntryState &s) {
        auto element = text(s.label);
        if (s.focused) {
            element |= bold;
        }
        return element | center | borderEmpty | flex;
    };
    return option;
}

void Game::PopulateEnemies(std::ifstream enemiesJson) {
    nlohmann::json data = nlohmann::json::parse(enemiesJson);
    int count = 1; //0 is the player
    for (const auto &i: data) {
        if (!i.contains("defaultTurns") || !i.contains("currentTurns") || !i.contains("name")) {
            throw ObjectFail("Enemy");
        }

        Enemy enemy{i["defaultTurns"], i["currentTurns"], count, i["name"]};
        Enemies.push_back(std::make_shared<Enemy>(enemy));
        count++;
    }
}

void Game::PopulateGenerals(std::ifstream generalsJson) {
    nlohmann::json data = nlohmann::json::parse(generalsJson);
    int SGcount = 0, PGcount = 0, CGcount = 0, WGcount = 0, EGcount = 0;
    for (const auto &i: data) {
        if (!i.contains("firstName") || !i.contains("lastName") || !i.contains("type") || !i.contains("rarity") || !i.
            contains("armour") || !i.contains("strength") || !i.contains("melee") || !i.contains("ranged") || !i.
            contains("accuracy") || !i.contains("dexterity")) {
            throw ObjectFail("General");
        }

        General general{
            i["firstName"], i["lastName"], i["type"], i["rarity"],
            i["melee"], i["ranged"], i["armour"],
            i["strength"], i["accuracy"], i["dexterity"]
        };

        switch (general.getType()) {
            case 0: {
                general.setIndex(SGcount);
                StartingGenerals.push_back(general.clone());
                SGcount++;
                break;
            }
            case 1: {
                general.setIndex(PGcount);
                PlayerGenerals.push_back(general.clone());
                PGcount++;
                break;
            }
            case 2: {
                general.setIndex(CGcount);
                ContenderGenerals.push_back(general.clone());
                CGcount++;
                break;
            }
            case 3: {
                general.setIndex(WGcount);
                WarlordGenerals.push_back(general.clone());
                WGcount++;
                break;
            }
            case 4: {
                general.setIndex(EGcount);
                EmperorGenerals.push_back(general.clone());
                EGcount++;
                break;
            }
            default: {
                std::cerr << "Unhandled type, check your .json! Game will start anyway." << std::endl;
            }
        }
    }
    generalsJson.close();

    //The emperor is hard-coded just because
    Emperor emp{
        "PIBBLE", "",
        777,
        999,
        99,
        99,
        99,
        99,
        99,
        99
    };
    EmperorUnit = std::make_shared<Emperor>(emp);
}

void Game::PopulateSettlements(std::ifstream settlementsJson) {
    nlohmann::json data = nlohmann::json::parse(settlementsJson);
    int count = 0;
    for (const auto &i: data) {
        if (!i.contains("startingGarrisonLevel")) {
            throw ObjectFail("Garrison");
        }

        if (!i.contains("name") || !i.contains("owner") || !i.contains("income")) {
            std::cout << i << std::endl;
            throw ObjectFail("Settlement");
        }

        Garrison garrison(i["startingGarrisonLevel"]); //Create the garrison according to config
        auto settlement = std::make_shared<Settlement>(garrison, i["name"], i["owner"], count, i["income"]);

        //Settlement is created and added to this collection
        Settlements.push_back(settlement);
        //The settlement receives a ptr to itself (useful for transfers)
        Settlements.back()->setSelfPtr(Settlements.back());

        //The enemy receives a pointer to that settlement (from the vector)
        if (static_cast<int>(i["owner"]) > 0) {
            Enemies[static_cast<int>(i["owner"]) - 1]->ModifySettlementOwnership(Settlements[count]);
        }
        count++;
    }
    settlementsJson.close();
}

void Game::PopulateControlPoints(std::ifstream controlPointsJson) const {
    nlohmann::json data = nlohmann::json::parse(controlPointsJson);
    for (const auto &i: data) {
        if (!i.contains("scoutViewRange")) {
            throw ObjectFail("Scout");
        }
        if (!i.contains("name") || !i.contains("cost") || !i.contains("ownedBy") || !i.contains("connectedTo")) {
            throw ObjectFail("ControlPoint");
        }

        Scout scout{i["scoutViewRange"]};
        ControlPoint controlPoint{scout, i["name"], i["cost"], i["ownedBy"], i["connectedTo"]};

        Settlements[i["ownedBy"]]->AddControlPoint(controlPoint);
        //I should also let the connected settlement know it has access to that controlPoint
        Settlements[i["connectedTo"]]->AddControlPoint(controlPoint);

        //Add the connection to both the Settlements' neighbour list
        Settlements[i["ownedBy"]]->AddNeighbour(Settlements[i["connectedTo"]]);
        Settlements[i["connectedTo"]]->AddNeighbour(Settlements[i["ownedBy"]]);
    }
    controlPointsJson.close();
}

void Game::PopulateCaptains(std::ifstream captainsJson) {
    nlohmann::json data = nlohmann::json::parse(captainsJson);
    for (const auto &i: data) {
        if (!i.contains("firstName") || !i.contains("lastName") || !i.contains("type") || !i.contains("rarity") || !i.
            contains("armour") || !i.contains("strength") || !i.contains("melee") || !i.contains("ranged") || !i.
            contains("accuracy") || !i.contains("dexterity")) {
            throw ObjectFail("Captain");
        }

        Captain captain{
            i["firstName"], i["lastName"], i["type"], i["rarity"],
            i["melee"], i["ranged"], i["armour"],
            i["strength"], i["accuracy"], i["dexterity"], captainInitialHandicapMultiplier
        };

        Captains.push_back(captain.clone());
    }
    captainsJson.close();
}

void Game::PopulateLocalLeaders(std::ifstream localLeadersJson) {
    nlohmann::json data = nlohmann::json::parse(localLeadersJson);
    for (const auto &i: data) {
        if (!i.contains("firstName") || !i.contains("lastName") || !i.contains("type") || !i.contains("rarity") || !i.
            contains("armour") || !i.contains("strength") || !i.contains("melee") || !i.contains("ranged") || !i.
            contains("accuracy") || !i.contains("dexterity") || !i.contains("incomeMultiplier") || !i.contains(
                "battleHandicap")) {
            throw ObjectFail("General");
        }
        LocalLeader leader{
            i["firstName"], i["lastName"], i["type"], i["rarity"],
            i["melee"], i["ranged"], i["armour"],
            i["strength"], i["accuracy"], i["dexterity"],
            i["incomeMultiplier"], i["battleHandicap"]
        };
        LocalLeaders.push_back(leader.clone());
    }
    localLeadersJson.close();
}

void Game::InitializeArmiesAndSettlements() const {
    //Create the initial enemy armies
    //Army that should beat the player
    Army warlord1VlascaArmyInitial{Captains[0]};
    warlord1VlascaArmyInitial.AddUnit(WarlordGenerals[3]);
    warlord1VlascaArmyInitial.AddUnit(WarlordGenerals[68]);

    Settlements[1]->StationArmy(std::make_shared<Army>(warlord1VlascaArmyInitial));
    //The second army of Vlasca is made in the code, after the first one is deleted.

    Army warlord1ColonistiArmyInitial{Captains[2]};
    warlord1ColonistiArmyInitial.AddUnit(WarlordGenerals[4]);
    warlord1ColonistiArmyInitial.AddUnit(WarlordGenerals[20]);

    //Settlements[3]->StationArmy(std::make_shared<Army>(warlord1ColonistiArmyInitial));
    //TEMPORARY local leader test
    Settlements[3]->AppointLocalLeader(LocalLeaders[2]);

    /*
    Army warlord2FetestiArmyInitial{Captains[4]};
    warlord2FetestiArmyInitial.AddUnit(WarlordGenerals[5]);
    warlord2FetestiArmyInitial.AddUnit(WarlordGenerals[19]);

    Settlements[2]->StationArmy(std::make_shared<Army>(warlord2FetestiArmyInitial));
    */

    //TEMPORARY emperor test
    Army emperorArmyInitial{EmperorUnit};
    Settlements[2]->StationArmy(std::make_shared<Army>(emperorArmyInitial));


    //Make the first enemy discovered
    Enemies[0]->Discovered();
}

void Game::CheckGenerals() const {
    std::cout << "The generals.json file currently holds " << StartingGenerals.size() + PlayerGenerals.size() +
            ContenderGenerals.size() + WarlordGenerals.size() + EmperorGenerals.size() << " generals." << std::endl;
    std::cout << "Starter generals for the player: " << StartingGenerals.size() << std::endl;
    std::cout << "Generals selectable by the player (at any point): " << StartingGenerals.size() + PlayerGenerals.size()
            << std::endl;
    std::cout << "Contender general pool (doesn't use more than 3): " << ContenderGenerals.size() << std::endl;
    std::cout << "Warlord general pool: " << WarlordGenerals.size() << std::endl;
    std::cout << "Emperor general pool: " << EmperorGenerals.size() << std::endl;
}

void Game::ShowPlayerGenerals() const {
    for (unsigned long i = 0; i < StartingGenerals.size(); i++) {
        std::cout << i << std::endl;
        std::cout << *StartingGenerals[i] << std::endl;
    }
    for (unsigned long i = 0; i < PlayerGenerals.size(); i++) {
        std::cout << i << std::endl;
        std::cout << *PlayerGenerals[i] << std::endl;
    }
}

ftxui::Table Game::CreateStartingGeneralsTable() const {
    auto Generals = StartingGenerals;
    std::vector<std::string> statsToPrintForEachGeneral;
    std::vector<std::vector<std::string> > tableContent;
    tableContent.push_back(generalTableHeaders);

    int count = 0;
    for (const auto &general: Generals) {
        std::string countConverted = std::to_string(count);

        statsToPrintForEachGeneral.clear();
        statsToPrintForEachGeneral = general->getPrintableStats();
        statsToPrintForEachGeneral.emplace(statsToPrintForEachGeneral.begin(), countConverted);
        tableContent.push_back(statsToPrintForEachGeneral);

        count++;
    }

    using namespace ftxui;
    auto table = Table({tableContent});

    table.SelectAll().Border(LIGHT);

    //Make first row bold with a double border.
    table.SelectRow(0).Decorate(bold);
    table.SelectRow(0).Border(DOUBLE);

    //Separators
    table.SelectAll().SeparatorVertical(LIGHT);
    table.SelectAll().SeparatorHorizontal(LIGHT);

    // elect row from the second to the last.
    auto content = table.SelectRows(1, -1);
    //Alternate in between 3 colors.
    content.DecorateCellsAlternateRow(color(Color::Blue), 3, 0);
    content.DecorateCellsAlternateRow(color(Color::Cyan), 3, 1);
    content.DecorateCellsAlternateRow(color(Color::White), 3, 2);

    return table;
}

void Game::DisplayStartingGenerals() const {
    using namespace ftxui;

    Table table = CreateStartingGeneralsTable();

    auto document = table.Render();

    //Legacy rendering logic for the normal branch
    auto screen =
            Screen::Create(Dimension::Fit(document, /*extend_beyond_screen=*/true));
    //Non-blocking output, I don't want to have to output every single thing using FTXUI and its messy scrolling, maybe later
    Render(screen, document);
    screen.Print();
    std::cout << std::endl;
}

ftxui::Element Game::FTXUIDisplayStaringGenerals() const {
    using namespace ftxui;

    Table table = CreateStartingGeneralsTable();

    auto document = table.Render();

    //New rendering logic
    return document;
}

void Game::FTXUIDisplaySettlementAndArmy(const ftxui::Component &whereToDisplay, const Settlement &settlement) {
    using namespace ftxui;
    AddElementToFTXUIContainer(whereToDisplay, settlement.FTXUIDisplaySettlement());
    if (settlement.getStationedArmy().has_value() && settlement.getStationedArmy().value()->getUnitCount() > 0) {
        AddElementToFTXUIContainer(whereToDisplay, paragraph("With the stationed army:"));
        AddElementToFTXUIContainer(whereToDisplay,
                                   (*settlement.getStationedArmy())->FTXUIDisplayArmy());
    } else {
        AddElementToFTXUIContainer(whereToDisplay, paragraph("With no stationed army."));
    }
    AddNewLineToFTXUIContainer(whereToDisplay);
}

void Game::FTXUIDisplayOnlyArmyFromSettlement(const ftxui::Component &whereToDisplay, const Settlement &settlement) {
    using namespace ftxui;
    if (settlement.getStationedArmy().has_value()) {
        AddElementToFTXUIContainer(whereToDisplay,
                                   (*settlement.getStationedArmy())->FTXUIDisplayArmy());
        AddNewLineToFTXUIContainer(whereToDisplay);
    }
}

void Game::FTXUIDisplayOnlyPlayerSettlementsWithArmies(const ftxui::Component &whereToDisplay) const {
    for (const auto &settlement: Settlements) {
        if (settlement->getOwner() == 0 && settlement->getStationedArmy().has_value()) {
            FTXUIDisplaySettlementAndArmy(whereToDisplay, *settlement);
        }
    }
}

ftxui::Table Game::CreatePlayerAdditionalGeneralsTable() const {
    //I can't use the already existing StartingGenerals table because it is not modifiable after creation
    //I'll just create a headerless table with the other generals and display it as well
    using namespace ftxui;
    std::vector<std::string> statsToPrintForEachGeneral;
    std::vector<std::vector<std::string> > tableContent;
    auto AdditionalGenerals = PlayerGenerals;
    tableContent.push_back(generalTableHeaders);

    unsigned long count = StartingGenerals.size();
    for (const auto &general: AdditionalGenerals) {
        std::string countConverted = std::to_string(count);

        statsToPrintForEachGeneral.clear();
        statsToPrintForEachGeneral = general->getPrintableStats();
        statsToPrintForEachGeneral.emplace(statsToPrintForEachGeneral.begin(), countConverted);
        tableContent.push_back(statsToPrintForEachGeneral);

        count++;
    }

    auto table = Table({tableContent});

    table.SelectAll().Border(LIGHT);

    //Make first row bold with a double border.
    table.SelectRow(0).Decorate(bold);
    table.SelectRow(0).Border(DOUBLE);

    //Separators
    table.SelectAll().SeparatorVertical(LIGHT);
    table.SelectAll().SeparatorHorizontal(LIGHT);

    // elect row from the second to the last.
    auto content = table.SelectRows(1, -1);
    //Alternate in between 3 colors.
    content.DecorateCellsAlternateRow(color(Color::Blue), 3, 0);
    content.DecorateCellsAlternateRow(color(Color::Cyan), 3, 1);
    content.DecorateCellsAlternateRow(color(Color::White), 3, 2);

    return table;
}

ftxui::Element Game::FTXUIDisplayAdditionalPlayerGenerals() const {
    using namespace ftxui;

    Table table = CreatePlayerAdditionalGeneralsTable();

    auto document = table.Render();

    //New rendering logic
    return document;
}

ftxui::Table Game::CreateCaptainsTable() const {
    using namespace ftxui;
    std::vector<std::string> statsToPrintForEachCaptain;
    std::vector<std::vector<std::string> > tableContent;
    tableContent.push_back(generalTableHeaders);

    unsigned long count = StartingGenerals.size() + PlayerGenerals.size();
    for (const auto &captain: Captains) {
        std::string countConverted = std::to_string(count);

        statsToPrintForEachCaptain.clear();
        statsToPrintForEachCaptain = captain->getPrintableStats();
        statsToPrintForEachCaptain.emplace(statsToPrintForEachCaptain.begin(), countConverted);
        tableContent.push_back(statsToPrintForEachCaptain);

        count++;
    }

    auto table = Table({tableContent});

    table.SelectAll().Border(LIGHT);

    //Make first row bold with a double border.
    table.SelectRow(0).Decorate(bold);
    table.SelectRow(0).Border(DOUBLE);

    //Separators
    table.SelectAll().SeparatorVertical(LIGHT);
    table.SelectAll().SeparatorHorizontal(LIGHT);

    // elect row from the second to the last.
    auto content = table.SelectRows(1, -1);
    //Alternate in between 3 colors.
    content.DecorateCellsAlternateRow(color(Color::Blue), 3, 0);
    content.DecorateCellsAlternateRow(color(Color::Cyan), 3, 1);
    content.DecorateCellsAlternateRow(color(Color::White), 3, 2);

    return table;
}

ftxui::Element Game::FTXUIDisplayCaptains() const {
    using namespace ftxui;

    Table table = CreateCaptainsTable();

    auto document = table.Render();

    //New rendering logic
    return document;
}


void Game::ResetArmiesActionPoints() const {
    for (const auto &settlement: Settlements) {
        //If the settlement has an army
        if (settlement->getStationedArmy() != std::nullopt) {
            //We reset that army's action points to the default value
            (*settlement->getStationedArmy())->resetActionPoints();
        }
    }
}

void Game::CollectIncomeFromSettlements() {
    for (const auto &i: Settlements) {
        //If the settlement is owned by the player
        if (i->getOwner() == 0) {
            //We add that income (or subtract if it's negative, duh) to the total balance of the player
            sarmale += i->getIncome();
        }
    }
}

void Game::NextTurn() {
    currentTurn++;
    ResetArmiesActionPoints();
    CollectIncomeFromSettlements();
    //Enemy related stuff
    for (const auto &enemy: Enemies) {
        enemy->AdvanceTurn(gameWindow);
    }
    //so it doesn't say empty container
    AddNewLineToFTXUIContainer(gameWindow);
}

void Game::ShowMenu() const {
    using namespace ftxui;

    auto screen = ScreenInteractive::FitComponent();

    //button variables so I can use them in functions
    Component startGameButton, exitButton, loadGameButton;

    //parent container for everything
    auto menuContainer = Container::Horizontal({});

    auto introContainer = Container::Vertical({});
    auto buttonsContainer = Container::Vertical({});


    //paragraphs
    auto introductionParagraph = paragraph("PARTIAL CONFLICT: LEGENDS") | center | bold | color(honeydew);

    //button-related
    //STYLES are done throught the helper function

    //FUNCTIONS
    auto onStartGameButtonClicked = [&] {
        screen.Exit();
    };

    auto onLoadGameButtonClicked = [&] {
        //not working temporarily
        screen.Exit();
    };

    auto onExitButtonClicked = [&] {
        screen.Exit();
        throw(ApplicationException("Premature exit triggered by player"));
    };

    //button definition
    startGameButton = Button("Start a New Game", onStartGameButtonClicked,
                             ButtonStyleCenterText(kaki, susPink, backgroundGrey, backgroundGrey))
                      | size(WIDTH, GREATER_THAN, Terminal::Size().dimx / 100.0f * 35)
                      | size(HEIGHT, GREATER_THAN, Terminal::Size().dimy / 100.0f * 10);

    loadGameButton = Button("Load Progress", onLoadGameButtonClicked,
                            ButtonStyleCenterText(beautifulBlue, beautifulGreen, backgroundGrey, backgroundGrey))
                     | size(WIDTH, GREATER_THAN, Terminal::Size().dimx / 100.0f * 35)
                     | size(HEIGHT, GREATER_THAN, Terminal::Size().dimy / 100.0f * 10);

    exitButton = Button("Exit", onExitButtonClicked,
                        ButtonStyleCenterText(weirdPurple, beautifulOrange, backgroundGrey, backgroundGrey))
                 | size(WIDTH, GREATER_THAN, Terminal::Size().dimx / 100.0f * 35)
                 | size(HEIGHT, GREATER_THAN, Terminal::Size().dimy / 100.0f * 10);


    //add all components to parents
    //paragraphs
    AddElementToFTXUIContainer(introContainer, introductionParagraph);

    //buttons
    AddMoreLinesToFTXUIContainer(buttonsContainer);
    buttonsContainer->Add(startGameButton);
    AddNewLineToFTXUIContainer(buttonsContainer);
    buttonsContainer->Add(loadGameButton);
    AddNewLineToFTXUIContainer(buttonsContainer);
    buttonsContainer->Add(exitButton);
    AddMoreLinesToFTXUIContainer(buttonsContainer);

    menuContainer->Add(introContainer);
    menuContainer->Add(buttonsContainer);

    auto renderer = Renderer(menuContainer, [&] {
        return vbox({
                   introContainer->Render(),
                   separator(),

                   buttonsContainer->Render() | center,
               }) | size(WIDTH, EQUAL, Terminal::Size().dimx);
    });

    screen.Loop(renderer);
}

bool Game::SaveGame() {
    time_t timestamp;
    time(&timestamp);
    std::ofstream saveFile("Save " + std::string(ctime(&timestamp)));
    //the output in json formatting
    nlohmann::json jsonArray = nlohmann::json::array();

    if (!saveFile.is_open()) {
        throw(BrokenFile("Save file"));
    }
    //else it's ok
    //Go through each Settlement and save their owner and index, along with the stationedArmy (if it's the case)
    for (const auto &settlement: Settlements) {
        nlohmann::json jsonFormatSaveFile;
        auto index = settlement->getIndex();
        auto owner = settlement->getOwner();

        jsonFormatSaveFile = {
            {"settlementIndex", index},
            {"owner", owner}
        };

        auto army = settlement->getStationedArmy();
        if (army.has_value()) {
            jsonFormatSaveFile.push_back({"unitsInArmy", army.value()->getUnitCount()});
            int count = 0;
            for (const auto &unit: army.value()->getAssignedUnits()) {
                auto unitIndex = unit->getIndex();
                auto unitType = unit->getType();
                jsonFormatSaveFile.push_back({"unitIndex" + std::to_string(count), unitIndex});
                jsonFormatSaveFile.push_back({"unitType" + std::to_string(count), unitType});
                count++;
            }
        } else {
            jsonFormatSaveFile.push_back({"unitsInArmy", 0});
        }

        jsonArray.push_back(jsonFormatSaveFile);
    }
    saveFile << jsonArray;
    return true;
}

void Game::ReplaceAllButtonsWithAnother(const ftxui::Component &container, const ftxui::Component &button) {
    container->DetachAllChildren();
    container->Add(button);
}

//ultimate code sausage / a really long piece of spaghetti
int Game::Start() {
    std::ifstream generalsJson, settlementsJson, controlPointsJson, captainsJson, enemiesJson, localLeadersJson;

    generalsJson.open("generals.json");
    settlementsJson.open("settlements.json");
    controlPointsJson.open("controlPoints.json");
    captainsJson.open("captains.json");
    enemiesJson.open("enemies.json");
    localLeadersJson.open("localLeaders.json");

    if (!generalsJson) {
        throw BrokenFile("generals.json");
    }
    if (!settlementsJson) {
        throw BrokenFile("settlements.json");
    }
    if (!captainsJson) {
        throw BrokenFile("captains.json");
    }
    if (!controlPointsJson) {
        throw BrokenFile("controlPoints.json");
    }
    if (!enemiesJson) {
        throw BrokenFile("enemies.json");
    }
    if (!localLeadersJson) {
        throw BrokenFile("localLeaders.json");
    }
    PopulateEnemies(std::move(enemiesJson));
    PopulateGenerals(std::move(generalsJson));
    PopulateSettlements(std::move(settlementsJson));
    PopulateControlPoints(std::move(controlPointsJson));
    PopulateCaptains(std::move(captainsJson));
    PopulateLocalLeaders(std::move(localLeadersJson));

    if (WarlordGenerals.size() < warlordMinimumGenerals) {
        std::cout << warlordCountWarningText;
        return -1;
    }
    if (EmperorGenerals.size() < emperorMinimumGenerals) {
        std::cout << emperorCountWarningText;
        return -1;
    }

    InitializeArmiesAndSettlements();

    //TEMPORARILY UNDER CONSTRUCTION

    //NEW STUFF FOR PLAYING THE GAME

    //Initial welcome screen and prompts to check stuff or start the game
    OutputFTXUIText(welcomeText, gameAnnouncementsColor);
    OutputFTXUIText(balanceCheckText, userInputExpectedColor);
    std::cin >> ans1;
    sanitizeInputMore(ans1);
    if (ans1 > 1) {
        ans1 = 0;
    } else if (ans1 == 1) {
        std::string temp;
        CheckGenerals();
        OutputFTXUIText("Would you like to see a list of the player's generals?\n", userInputExpectedColor);
        std::cin >> ans3;
        sanitizeInputMore(ans3);
        if (ans3 > 1) {
            ans3 = 0;
        } else {
            ShowPlayerGenerals();
        }
        OutputFTXUIText(enterToContinueText, userInputExpectedColor);
        std::cin.ignore(); //Flush \n from the buffer
        std::getline(std::cin, temp); //Wait until the player has read the list / wants to continue
    }
    std::cout << "\n";

    OutputFTXUIText(ftxuiConfirmationText, userInputExpectedColor);
    std::cin >> ans1;
    std::cout << "\n\n\n";
    sanitizeInputMore(ans1);
    if (ans1 > 1) {
        ans1 = 1;
    } else if (ans1 == 1) {
        //FTXUI branch
        /*BRIEF EXPLANATION AFTER FUCKING AROUND AND FINDING OUT:
        *  The bigger container that is gameContainer HAS to have all I want to be in it AS CHILDREN.
        *  The stuff I do in renderer is just for choosing how to properly display what I want to.
        *  If the things I want to display are not children of the component written in Renderer(..., lambda function),
        *  then I am only able to display their "skeletons", because the functionality will not be there.
        *
        *  VERY IMPORTANT:
        *  If a container has elements (e.g. text, paragraph), not components (e.g. buttons) in it, then it is inherently NOT SCROLLABLE!
        *  This can be fixed by appending (with '|') a focusPositionRelative to the container,
        *  then appending a CatchEvent to the renderer that uses the Mouse Wheel to increment or decrement this relative position
        */
        using namespace ftxui;

        //show menu first
        ShowMenu();

        //where to store input
        std::string tempInput, modifiedArmyInputString, moveArmyInputString;

        //variables that will be used
        unsigned long startingGeneralChosenIndex = 0, whichArmyToModify = 0, moveArmyFromIndex = 0;
        int timesWithoutSettlements = 0;
        bool checkSettlementClickedFirstTime = false, checkEnemyIntentsClickedCurrentTurn = false;
        std::vector<std::shared_ptr<Army> > PlayerArmies;
        std::vector<unsigned long> validIndexes; //for moving an army - valid neighbours

        //button variables so I can use them in functions
        Component testButton, checkSettlementsButton, checkEnemyIntentsButton, modifyPlayerArmyButton, nextTurnButton,
                exitButton, moveArmyButton, saveGameButton;

        //to scroll text because it is insanely hard apparently
        float focus_y = 0.5f;
        float step = 0.08f;
        float upperLimit = 1.f;
        float lowerLimit = 0.f;

        auto screen = ScreenInteractive::FitComponent(); //a responsive screen that fits the terminal

        //container where all the feedback is - made scrollable using | focusPositionRelative
        gameWindow = Container::Vertical({});

        //container with the game control buttons I want to use
        auto gameStateButtonsContainer = Container::Horizontal({});

        //container to have all things related to game display in it
        auto gameContainer = Container::Horizontal({});

        //container for the contextual buttons that can be used
        auto gameContextualButtonsContainer = Container::Horizontal({});

        //for inputs
        InputOption inputOption = InputOption::Spacious();
        inputOption.transform = [](InputState state) {
            state.element |= color(userInputExpectedColor);
            if (state.focused) {
                state.element |= bgcolor(Color::Default);
            }
            else if (state.hovered) {
                state.element |= bgcolor(Color::Grey15);
            }
            else {
                state.element |= bgcolor(Color::Grey27);
            }
            return state.element;
        };

        //INPUTS

        //Every time I want to listen to input from the user, I will have to add an input such as this one
        Component starterGeneralInput = Input(&tempInput, starterPreChoiceText, inputOption)
                                        | size(HEIGHT, GREATER_THAN, Terminal::Size().dimy / 100.0f * 5);
        Component modifyArmyCountInput = Input(&modifiedArmyInputString, "Count of the army you want to modify:",
                                               inputOption)
                                         | size(HEIGHT, GREATER_THAN, Terminal::Size().dimy / 100.0f * 5);
        Component modifyArmyWhatInput = Input(&modifiedArmyInputString,
                                              "0 - ADD UNIT | 1 - REMOVE UNIT | 2 - DELETE ARMY | 3 - CANCEL",
                                              inputOption)
                                        | size(HEIGHT, GREATER_THAN, Terminal::Size().dimy / 100.0f * 5);
        Component modifyArmyAddInput = Input(&modifiedArmyInputString, "Index of the general you'd wish to add:",
                                             inputOption)
                                       | size(HEIGHT, GREATER_THAN, Terminal::Size().dimy / 100.0f * 5);
        Component modifyArmyRemoveInput = Input(&modifiedArmyInputString, "Index of the general you want to remove:",
                                                inputOption)
                                          | size(HEIGHT, GREATER_THAN, Terminal::Size().dimy / 100.0f * 5);
        Component moveArmyWhichSettlementInput = Input(&moveArmyInputString, "Index of the settlement", inputOption)
                                                 | size(HEIGHT, GREATER_THAN, Terminal::Size().dimy / 100.0f * 5);
        Component moveArmyWhereInput = Input(&moveArmyInputString, "Index of the neighbour:", inputOption)
                                       | size(HEIGHT, GREATER_THAN, Terminal::Size().dimy / 100.0f * 5);
        //CATCH EVENTS FOR INPUTS

        //because I have to only catch events that are related to input, not mouse hovers, clicks and other stuff,
        //I can only return true on what I am certain I don't want, then return false for anything else.
        starterGeneralInput |= CatchEvent([&](const Event &event) {
            if (event.is_character() && !std::isdigit(event.character()[0])) {
                return true; //it's not a digit, catch it and prevent it from modifying tempInput
            }
            return false; //it's a digit
        });
        //I only want to prevent/repurpose enter, anything else can go through (and to other catchers, eventually)
        starterGeneralInput |= CatchEvent([&](const Event &event) {
            if (event == Event::Return) {
                if (!tempInput.empty()) {
                    startingGeneralChosenIndex = std::stoul(tempInput); //try to parse as unsigned long
                    if (startingGeneralChosenIndex >= StartingGenerals.size()) {
                        //too high, reset and try again
                        AddElementToFTXUIContainer(
                            gameWindow, paragraph(
                                "try again! min: 0, max: " + std::to_string(StartingGenerals.size() - 1)));
                        tempInput = "";
                    } else {
                        //in range, can proceed
                        gameWindow->DetachAllChildren(); //remove text that becomes useless

                        Army starterArmy{StartingGenerals[startingGeneralChosenIndex]};
                        //temp (so that the selection screen is actually usable
                        starterArmy.AddUnit(PlayerGenerals[10]);
                        starterArmy.AddUnit(PlayerGenerals[33]);
                        Settlements[0]->StationArmy(std::make_shared<Army>(starterArmy));
                        AddElementToFTXUIContainer(
                            gameWindow,
                            paragraph("You should check out your settlements now!") | color(
                                importantGameInformationColor));
                        gameContextualButtonsContainer->Add(checkSettlementsButton);
                    }
                    focus_y = upperLimit;
                }
                return true; //Catch the enter and do something else
            }
            return false; //Don't mess with any other event
        });

        modifyArmyRemoveInput |= CatchEvent([&](const Event &event) {
            if (event.is_character() && !std::isdigit(event.character()[0])) {
                return true; //it's not a digit, catch it and prevent it from modifying tempInput
            }
            return false; //it's a digit
        });

        modifyArmyRemoveInput |= CatchEvent([&](const Event &event) {
            if (event == Event::Return) {
                if (!modifiedArmyInputString.empty()) {
                    unsigned long whichUnitToRemove = std::stoul(modifiedArmyInputString);

                    modifiedArmyInputString = "";
                    if (whichUnitToRemove > PlayerArmies[whichArmyToModify]->getUnitCount()) {
                        //invalid, can't remove something that doesn't exist
                        return true;
                    }
                    PlayerArmies[whichArmyToModify]->RemoveUnit(whichUnitToRemove);

                    AddElementToFTXUIContainer(
                        gameWindow, paragraph("You have successfully modified your army! You should check it out :)"));

                    modifyArmyRemoveInput->Detach();
                }
                return true; //Catch the enter and do something else
            }
            return false; //Don't mess with any other event
        });

        modifyArmyAddInput |= CatchEvent([&](const Event &event) {
            if (event.is_character() && !std::isdigit(event.character()[0])) {
                return true; //it is a character, it's not a digit, catch it and prevent it from modifying tempInput
            }
            return false; //it's a digit
        });

        modifyArmyAddInput |= CatchEvent([&](const Event &event) {
            if (event == Event::Return) {
                if (!modifiedArmyInputString.empty()) {
                    bool addSuccess = true;
                    std::shared_ptr<Unit> addedUnit;
                    unsigned long whichUnitToAdd = std::stoul(modifiedArmyInputString); //try to parse as unsigned long
                    //reset for future use
                    modifiedArmyInputString = "";
                    if (whichUnitToAdd > StartingGenerals.size() + PlayerGenerals.size() + Captains.size() - 1) {
                        //invalid range
                        return true;
                    }
                    //Add this unit to the Army
                    if (whichUnitToAdd < StartingGenerals.size()) {
                        addedUnit = StartingGenerals[whichUnitToAdd];
                    } else if (whichUnitToAdd < StartingGenerals.size() + PlayerGenerals.size()) {
                        //transform from big number to smaller number that fits the actual vector
                        whichUnitToAdd -= StartingGenerals.size();
                        addedUnit = PlayerGenerals[whichUnitToAdd];
                    } else {
                        whichUnitToAdd -= StartingGenerals.size();
                        whichUnitToAdd -= PlayerGenerals.size();
                        addedUnit = Captains[whichUnitToAdd];
                    }
                    try {
                        PlayerArmies[whichArmyToModify]->AddUnit(PlayerGenerals[whichUnitToAdd]);
                    } catch (const SizeViolation &) {
                        //Because I can't output with std::cout in ftxui, I will define the custom behaviour here
                        AddElementToFTXUIContainer(gameWindow, paragraph("Army is full already!"));
                        addSuccess = false;
                    }
                    if (addSuccess) {
                        AddElementToFTXUIContainer(
                            gameWindow, paragraph("You have successfully modified your army - added " + addedUnit->getFullName()));
                    }


                    modifyArmyAddInput->Detach();
                }
                return true; //Catch the enter and do something else
            }
            return false; //Don't mess with any other event
        });

        modifyArmyWhatInput |= CatchEvent([&](const Event &event) {
            if (event.is_character() && !std::isdigit(event.character()[0])) {
                return true; //it is a character, it's not a digit, catch it and prevent it from modifying tempInput
            }
            return false; //it's a digit
        });

        modifyArmyWhatInput |= CatchEvent([&](const Event &event) {
            if (event == Event::Return) {
                if (!modifiedArmyInputString.empty()) {
                    const int option = std::stoi(modifiedArmyInputString); //try to parse as unsigned long
                    //reset for the next attempt or input
                    modifiedArmyInputString = "";
                    if (option > 3 || option < 0) {
                        //invalid options are ignored
                        return true;
                    }
                    switch (option) {
                        case 0: {
                            //add
                            //I want to inform the player about what choices he has
                            AddElementToFTXUIContainer(gameWindow, paragraph("Choose from this list of starters:"));
                            AddElementToFTXUIContainer(gameWindow, FTXUIDisplayStaringGenerals());
                            AddElementToFTXUIContainer(gameWindow, paragraph("Or from this list of non-starters:"));
                            AddElementToFTXUIContainer(gameWindow, FTXUIDisplayAdditionalPlayerGenerals());
                            AddElementToFTXUIContainer(gameWindow, paragraph("Or from this list of captains:"));
                            AddElementToFTXUIContainer(gameWindow, FTXUIDisplayCaptains());

                            gameWindow->Add(modifyArmyAddInput);
                            break;
                        }
                        case 1: {
                            //remove
                            gameWindow->Add(modifyArmyRemoveInput);
                            break;
                        }
                        case 2: {
                            //delete whole army
                            PlayerArmies[whichArmyToModify]->Disband();
                            AddElementToFTXUIContainer(gameWindow, paragraph("Army obliterated."));
                            break;
                        }
                        default: {
                            //cancel
                            AddElementToFTXUIContainer(gameWindow, paragraph("Nothing was done"));
                        }
                    }
                    modifyArmyWhatInput->Detach();
                }
                return true; //Catch the enter and do something else
            }
            return false; //Don't mess with any other event
        });

        modifyArmyCountInput |= CatchEvent([&](const Event &event) {
            if (event.is_character() && !std::isdigit(event.character()[0])) {
                return true; //it is a character, it's not a digit, catch it and prevent it from modifying tempInput
            }
            return false; //it's a digit
        });
        //I want to wait for the enter and do things after it's pressed
        modifyArmyCountInput |= CatchEvent([&](const Event &event) {
            if (event == Event::Return) {
                if (!modifiedArmyInputString.empty()) {
                    whichArmyToModify = std::stoul(modifiedArmyInputString); //try to parse as unsigned long

                    if (whichArmyToModify > PlayerArmies.size() - 1) {
                        //too big, doesn't exist
                        modifiedArmyInputString = "";
                    } else {
                        //remove and reset the count input
                        modifiedArmyInputString = "";
                        modifyArmyCountInput->Detach();
                        //inform the player
                        AddElementToFTXUIContainer(gameWindow, paragraph("You are modifying this army:"));
                        AddElementToFTXUIContainer(gameWindow, PlayerArmies[whichArmyToModify]->FTXUIDisplayArmy());
                        AddElementToFTXUIContainer(gameWindow, separator());
                        //add input to know what we want to do
                        gameWindow->Add(modifyArmyWhatInput);
                    }
                }
                return true; //Catch the enter and do something else
            }
            return false; //Don't mess with any other event
        });

        moveArmyWhichSettlementInput |= CatchEvent([&](const Event &event) {
            if (event.is_character() && !std::isdigit(event.character()[0])) {
                return true; //it is a character, it's not a digit, catch it and prevent it from modifying tempInput
            }
            return false; //it's a digit
        });

        moveArmyWhichSettlementInput |= CatchEvent([&](const Event &event) {
            if (event == Event::Return) {
                if (!moveArmyInputString.empty()) {
                    moveArmyFromIndex = std::stoul(moveArmyInputString); //try to parse as unsigned long

                    //reset for future use
                    moveArmyInputString = "";
                    if (moveArmyFromIndex >= Settlements.size()) {
                        //invalid input
                        return true;
                    }
                    auto originalSettlement = Settlements[moveArmyFromIndex];
                    if (originalSettlement->getOwner() != 0) {
                        //can't control enemy armies
                        return true;
                    }

                    if (originalSettlement->getStationedArmy().has_value() != true) {
                        //can't move an army that doesn't exist
                        return true;
                    }

                    auto Neighbours = originalSettlement->getNeighbours();
                    //inform the player about the possibilities
                    AddNewLineToFTXUIContainer(gameWindow);
                    AddElementToFTXUIContainer(
                        gameWindow, paragraph("Neighbours of settlement " + originalSettlement->getName() + ":"));
                    AddNewLineToFTXUIContainer(gameWindow);
                    for (const auto &neighbour: Neighbours) {
                        //initialize the vector of valid indexes of neighbours
                        validIndexes.push_back(neighbour->getIndex());
                        //show each neighbour
                        FTXUIDisplaySettlementAndArmy(gameWindow, *neighbour);

                    }
                    //moving the army means detaching it from its current settlement and sending it
                    //that will be done in the next input
                    //should use SendArmy or another func from Settlement!!!
                    AddNewLineToFTXUIContainer(gameWindow);
                    AddElementToFTXUIContainer(
                        gameWindow, paragraph("Now choose the index of the neighbour you want to move to."));
                    AddElementToFTXUIContainer(gameWindow, separator());
                    gameWindow->Add(moveArmyWhereInput);

                    moveArmyWhichSettlementInput->Detach();
                }
                return true; //Catch the enter and do something else
            }
            return false; //Don't mess with any other event
        });

        moveArmyWhereInput |= CatchEvent([&](const Event &event) {
            if (event.is_character() && !std::isdigit(event.character()[0])) {
                return true; //it is a character, it's not a digit, catch it and prevent it from modifying tempInput
            }
            return false; //it's a digit
        });

        moveArmyWhereInput |= CatchEvent([&](const Event &event) {
            if (event == Event::Return) {
                if (!moveArmyInputString.empty()) {
                    unsigned long moveArmyToIndex = std::stoul(moveArmyInputString); //try to parse as unsigned long

                    //reset for future use
                    moveArmyInputString = "";

                    const std::shared_ptr<Settlement> originalSettlement = Settlements[moveArmyFromIndex];
                    const std::shared_ptr<Settlement> targetSettlement = Settlements[moveArmyToIndex];

                    bool found = false;

                    for (const auto &index: validIndexes) {
                        if (moveArmyToIndex == index) {
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        //invalid input
                        AddElementToFTXUIContainer(gameWindow, paragraph("Invalid index"));
                        return true;
                    }
                    //else it's valid, and we should move the army to this neighbour, if it's allowed
                    if (targetSettlement->getOwner() == 0) {
                        //if we are trying to move to an allied settlement, check if it already has an army
                        if (targetSettlement->getStationedArmy().has_value()) {
                            //we can't move another army to it, retry
                            AddElementToFTXUIContainer(gameWindow, paragraph(
                                                           "Settlement" + std::to_string(moveArmyToIndex) +
                                                           "already has an army. Retry"));
                            moveArmyInputString = "";
                            return true;
                        }
                        //else, we can move the army there
                        bool sendRes = originalSettlement->MoveOwnArmyToAlliedSettlement(targetSettlement);

                        if (sendRes) {
                            AddElementToFTXUIContainer(
                                gameWindow, paragraph("Army moved successfully to an allied settlement!"));
                        } else {
                            AddElementToFTXUIContainer(
                                gameWindow, paragraph("Army couldn't be moved!"));
                        }
                    } else {
                        //attack
                        AddElementToFTXUIContainer(
                            gameWindow,
                            paragraph("This settlement isn't yours. This action will trigger an attack...")
                            | color(importantGameInformationColor));

                        std::vector<int> targetIndexes = {static_cast<int>(moveArmyToIndex)};
                        Enemy *neighbourEnemyOwner = Enemies[targetSettlement->getOwner() - 1].get();

                        const bool sendRes = originalSettlement->SendArmy(
                            originalSettlement->getStationedArmy().value(),
                            targetIndexes, neighbourEnemyOwner, gameWindow);
                        if (sendRes == true) {
                            //once it is sent successfully, it must also be removed
                            originalSettlement->DetachArmy();
                        } else {
                            AddElementToFTXUIContainer(
                                gameWindow, paragraph("Couldn't send army, probably not enough action points!"));
                        }
                    }
                    moveArmyWhereInput->Detach();
                }
                return true; //Catch the enter and do something else
            }
            return false; //Don't mess with any other event
        });

        //BUTTON STYLES

        auto nextTurnStyle = ButtonOption::Animated(Color::Default, Color::GrayDark,
                                                    Color::Default, Color::White);

        auto exitStyle = ButtonOption::Animated(Color::Default, Color::Orange1,
                                                Color::Default, Color::Red);

        auto testStyle = ButtonOption::Animated(Color::Default, Color::GrayDark,
                                                Color::Default, Color::White);

        auto checkSettlementsStyle = ButtonOption::Animated(Color::Default, beautifulBlue,
                                                            Color::Default, beautifulGreen);

        auto checkEnemyIntentsStyle = ButtonOption::Animated(Color::Default, beautifulOrange,
                                                             Color::Default, weirdPurple);

        auto modifyPlayerArmyStyle = ButtonOption::Animated(Color::Default, susPink,
                                                            Color::Default, kaki);

        auto moveArmyStyle = ButtonOption::Animated(Color::Default, olive,
                                                    Color::Default, grayYellow);

        auto saveGameStyle = ButtonOption::Animated(Color::Default, olive,
                                                    Color::Default, grayYellow);

        //FUNCTIONS FOR BUTTONS

        auto onNextTurnButtonClick = [&] {
            if (checkEnemyIntentsClickedCurrentTurn == true) {
                gameWindow->DetachAllChildren(); //clear last turn output
                NextTurn();
                focus_y = upperLimit; //So we see the last thing added

                //reset all checks
                checkEnemyIntentsClickedCurrentTurn = false;
            } else {
                AddElementToFTXUIContainer(gameWindow,
                                           paragraph("There still are things you should do!") | color(beautifulBlue));
            }
        };

        auto onExitButtonClick = [&] {
            screen.Exit();
        };

        auto onTestButtonClick = [&] {
            for (const auto &settlementPtr: Settlements) {
                AddElementToFTXUIContainer(gameWindow, settlementPtr->FTXUIDisplaySettlement());
            }
        };

        auto onCheckSettlementsButtonClick = [&] {
            AddNewLineToFTXUIContainer(gameWindow);
            AddElementToFTXUIContainer(gameWindow, separator());
            AddElementToFTXUIContainer(gameWindow, paragraph("CHECKING SETTLEMENTS") | center | color(beautifulGreen));
            AddNewLineToFTXUIContainer(gameWindow);
            AddElementToFTXUIContainer(gameWindow, paragraph("These are your settlements:"));
            //Get all player owned settlements and display their information
            int alliedSettlementCount = 0;
            for (unsigned long i = 0; i < Settlements.size(); i++) {
                if (Settlements[i]->getOwner() == 0) {
                    FTXUIDisplaySettlementAndArmy(gameWindow, *Settlements[i]);
                    alliedSettlementCount++;
                    focus_y = upperLimit; //auto-scroll to see the bottom of the output
                }
            }
            if (alliedSettlementCount == 0) {
                AddNewLineToFTXUIContainer(gameWindow);
                AddElementToFTXUIContainer(
                    gameWindow, paragraph("Wait... There are none! YOU LOST?!") | color(importantGameInformationColor));
                //The game won't end. I will give the player 1 more chance (insane lore).
                if (timesWithoutSettlements == 0) {
                    AddNewLineToFTXUIContainer(gameWindow);
                    AddElementToFTXUIContainer(gameWindow, paragraph("It can't be...") | color(storyRelatedTextColor));
                    AddElementToFTXUIContainer(
                        gameWindow,
                        paragraph("You were supposed to make a difference!") | color(storyRelatedTextColor));
                    AddElementToFTXUIContainer(gameWindow, paragraph("...") | color(storyRelatedTextColor));
                    AddElementToFTXUIContainer(
                        gameWindow,
                        paragraph("I won't allow you to escape this easily.") | color(storyRelatedTextColor));
                    AddElementToFTXUIContainer(
                        gameWindow,
                        paragraph("Rise again and show everyone why I chose you!") | color(storyRelatedTextColor));
                    AddNewLineToFTXUIContainer(gameWindow);

                    //Regiving the first settlement to the player and resetting the army

                    // -1 because the player is 0, and the enemies are in a vector from 0
                    int tempEnemyOwnerID = Settlements[0]->getOwner() - 1;
                    //remove from the enemy that temporarily occupied it
                    Enemies[tempEnemyOwnerID]->ModifySettlementOwnership(Settlements[0]);
                    Settlements[0]->GiveToPlayer(gameWindow);

                    Army starterArmy{StartingGenerals[startingGeneralChosenIndex]};
                    Settlements[0]->StationArmy(std::make_shared<Army>(starterArmy));

                    //Add new army to the first settlement the player will face
                    //Army that will replace and is actually beatable
                    Army warlord1VlascaArmy2{Captains[1]};
                    warlord1VlascaArmy2.AddUnit(WarlordGenerals[2]);
                    Settlements[1]->StationArmy(std::make_shared<Army>(warlord1VlascaArmy2));

                    //HINTS:
                    AddNewLineToFTXUIContainer(gameWindow);
                    AddElementToFTXUIContainer(gameWindow, paragraph("HINTS:"));
                    AddElementToFTXUIContainer(gameWindow, paragraph("1. AVOID PIBBLE;"));
                    AddElementToFTXUIContainer(gameWindow, paragraph("2. MAKE YOUR ARMY STRONGER;"));
                    AddElementToFTXUIContainer(gameWindow, paragraph(
                                                   "3. HURRY! (if Oculta Mondiala acts you will lose your settlement - maybe your army);"));
                    AddElementToFTXUIContainer(
                        gameWindow, paragraph(
                            "4. DON'T LOSE THIS ARMY! YOU MIGHT AS WELL EXIT IF IT HAPPENS (SORRY!)"));
                    AddNewLineToFTXUIContainer(gameWindow);


                    //Add the final contextual buttons
                    gameContextualButtonsContainer->Add(modifyPlayerArmyButton);
                    gameContextualButtonsContainer->Add(moveArmyButton);
                } else {
                    //the only available button becomes exit
                    ReplaceAllButtonsWithAnother(gameContextualButtonsContainer, exitButton);
                    ReplaceAllButtonsWithAnother(gameStateButtonsContainer, exitButton);
                }
                timesWithoutSettlements++;
            } else if (alliedSettlementCount > 3) {
                //TEMPORARY WIN
                //clear the screen and show the player it's over.
                gameWindow->DetachAllChildren();
                AddNewLineToFTXUIContainer(gameWindow);
                AddElementToFTXUIContainer(
                    gameWindow, paragraph("You won! (temporarily)") | color(storyRelatedTextColor));

                //only button becomes exit
                ReplaceAllButtonsWithAnother(gameContextualButtonsContainer, exitButton);
                ReplaceAllButtonsWithAnother(gameStateButtonsContainer, exitButton);
            }
            if (checkSettlementClickedFirstTime == false) {
                //after it being clicked the first time, we can continue the tutorial
                AddElementToFTXUIContainer(gameWindow,
                                           paragraph(" "));
                AddElementToFTXUIContainer(gameWindow,
                                           paragraph("Now take a look at your enemy's intents!") | color(beautifulOrange));
                gameContextualButtonsContainer->Add(checkEnemyIntentsButton);
            }
            checkSettlementClickedFirstTime = true;
        };

        auto onCheckEnemyIntentButtonClick = [&] {
            checkEnemyIntentsClickedCurrentTurn = true;

            //for more clarity
            AddNewLineToFTXUIContainer(gameWindow);
            AddElementToFTXUIContainer(gameWindow, separator());
            AddElementToFTXUIContainer(
                gameWindow, paragraph("CHECKING ENEMY INTENTS ") | center | color(beautifulOrange));

            //We can only get information about the enemies we currently have contact with.
            for (const auto &Enemy: Enemies) {
                if (Enemy->CheckShouldBeDiscovered() == true) {
                    std::vector<Settlement> enemySettlements = Enemy->getOwnedSettlements();
                    int turnsToAct = Enemy->getCurrentTurnsToAct();
                    std::string name = Enemy->getName();
                    AddNewLineToFTXUIContainer(gameWindow);
                    AddElementToFTXUIContainer(gameWindow, separator());
                    AddElementToFTXUIContainer(gameWindow, paragraph(name) | center | color(beautifulOrange));
                    AddElementToFTXUIContainer(gameWindow, separator());
                    AddNewLineToFTXUIContainer(gameWindow);
                    AddElementToFTXUIContainer(gameWindow,
                                               paragraph(
                                                   name + " intends to act in " + std::to_string(
                                                       turnsToAct) + " turn(s)."));
                    //If the discovered enemy is about to act, the player might get attacked.
                    //A player should check what settlements they have in contact with an enemy to know where to expect it.
                    if (turnsToAct == 1) {
                        AddElementToFTXUIContainer(gameWindow,
                                                   paragraph("You will likely be attacked!"));
                    }
                    AddNewLineToFTXUIContainer(gameWindow);
                    AddElementToFTXUIContainer(gameWindow,
                                               paragraph("Owned settlements:"));
                    for (const auto &settlement: enemySettlements) {
                        FTXUIDisplaySettlementAndArmy(gameWindow, settlement);
                    }
                }
            }
        };

        auto onModifyPlayerArmyButtonClick = [&] {
            int count = 0;
            Component armyDisplayContainer = Container::Horizontal({});
            //display all player-owned armies
            AddNewLineToFTXUIContainer(gameWindow);
            AddElementToFTXUIContainer(gameWindow, paragraph("These are your armies:"));
            for (const auto &settlement: Settlements) {
                if (settlement->getOwner() == 0 && settlement->getStationedArmy().has_value()) {
                    AddElementToFTXUIContainer(armyDisplayContainer,
                                               paragraph("Count = " + std::to_string(count)) | size(
                                                   WIDTH, GREATER_THAN, Terminal::Size().dimx / 100.0f * 10) | center);
                    FTXUIDisplayOnlyArmyFromSettlement(armyDisplayContainer, *settlement);
                    gameWindow->Add(armyDisplayContainer);
                    focus_y = upperLimit; //auto-scroll to see the bottom of the output
                    if (settlement->getStationedArmy() != std::nullopt) {
                        PlayerArmies.emplace_back(settlement->getStationedArmy().value());
                    }
                    count++;
                }
            }
            //add the input to gameWindow
            AddElementToFTXUIContainer(gameWindow, separator());
            gameWindow->Add(modifyArmyCountInput);
        };

        auto onMoveArmyButtonClick = [&] {
            //will list the settlements that have armies
            AddNewLineToFTXUIContainer(gameWindow);
            AddElementToFTXUIContainer(gameWindow, separator());
            AddElementToFTXUIContainer(gameWindow, paragraph(
                                                       "MOVING ARMY") | center | color(userInputExpectedColor));
            AddElementToFTXUIContainer(gameWindow, separator());
            AddNewLineToFTXUIContainer(gameWindow);
            AddElementToFTXUIContainer(gameWindow, paragraph(
                                           "These are your settlements with armies:"));
            FTXUIDisplayOnlyPlayerSettlementsWithArmies(gameWindow);
            AddElementToFTXUIContainer(gameWindow, paragraph(
                                           "Enter the index of the settlement whose army you'd like to move."));
            AddElementToFTXUIContainer(gameWindow, separator());
            //add the input
            gameWindow->Add(moveArmyWhichSettlementInput);

        };

        auto onSaveGameButtonClick = [&] {
            //save
            bool result = SaveGame();
            if (result == true) {
                AddElementToFTXUIContainer(gameWindow, paragraph("Game saved."));
            } else {
                AddElementToFTXUIContainer(gameWindow, paragraph("Save failed."));
            }
        };

        //GAME STATE CONTROL BUTTONS

        nextTurnButton = Button("Next Turn", onNextTurnButtonClick, nextTurnStyle);
        gameStateButtonsContainer->Add(nextTurnButton);

        AddHorizontalSpaceToFTXUIContainer(gameStateButtonsContainer);

        saveGameButton = Button("Save Game", onSaveGameButtonClick, saveGameStyle);
        gameStateButtonsContainer->Add(saveGameButton);

        AddHorizontalSpaceToFTXUIContainer(gameStateButtonsContainer);

        exitButton = Button("Exit", onExitButtonClick, exitStyle);
        gameStateButtonsContainer->Add(exitButton);


        //Adding all containers to the main one

        gameContainer->Add(gameWindow);
        gameContainer->Add(gameStateButtonsContainer);
        gameContainer->Add(gameContextualButtonsContainer);

        //Render the general layout of the game window
        auto renderer = Renderer(gameContainer, [&] {
            return vbox({
                       separator(),
                       hbox({
                           text("Current turn: ") | color(gameAnnouncementsColor),
                           text(std::to_string(currentTurn)),
                       }),
                       separator(),
                       gameContextualButtonsContainer->Render()
                       | frame
                       | size(HEIGHT, GREATER_THAN, Terminal::Size().dimy / 100.0f * 5),
                       separator(),
                       gameWindow->Render()
                       | focusPositionRelative(0.f, focus_y) //make it scrollable only on the y-axis
                       | vscroll_indicator //to indicate where we are
                       | frame //allows for a component to overflow with content (which is later made scrollable)
                       | size(HEIGHT, EQUAL, Terminal::Size().dimy / 100.0f * 85),
                       separator(),
                       gameStateButtonsContainer->Render() | size(WIDTH, EQUAL, Terminal::Size().dimx)
                       | frame
                       | size(HEIGHT, GREATER_THAN, Terminal::Size().dimy / 100.0f * 5),
                       separator(),
                   })
                   | size(WIDTH, EQUAL, Terminal::Size().dimx);
        });

        //Because I define my own scrolling logic, I have to add an Event Catcher to the renderer
        renderer |= CatchEvent([&](Event event) {
            if (event.is_mouse() && (event.mouse().button == Mouse::WheelUp ||
                                     event.mouse().button == Mouse::WheelDown)) {
                //Inverse because the terminal has 0 at the bottom and 1 at the top
                if (event.mouse().button == Mouse::WheelDown) {
                    focus_y = std::min(upperLimit, focus_y + step); //Go down (in the terminal)
                } else {
                    focus_y = std::max(lowerLimit, focus_y - step); //Go up (in the terminal)
                }
                return true;
            }
            return false;
        });

        //Actually displaying stuff in the gameFlowContainer
        //This is where I can basically add whatever needs to be shown to the player throughout the game.

        //Define contextual buttons that will be added progressively
        testButton = Button("Show ALL settlements", onTestButtonClick, testStyle);
        checkSettlementsButton = Button("Check my settlements", onCheckSettlementsButtonClick, checkSettlementsStyle);
        checkEnemyIntentsButton = Button("Check enemy intents", onCheckEnemyIntentButtonClick, checkEnemyIntentsStyle);
        modifyPlayerArmyButton = Button("Modify an army", onModifyPlayerArmyButtonClick, modifyPlayerArmyStyle);
        moveArmyButton = Button("Move an army", onMoveArmyButtonClick, moveArmyStyle);
        gameContextualButtonsContainer->Add(testButton);

        //Game intro
        AddElementToFTXUIContainer(gameWindow, paragraph(beginningGeneralText) | color(gameAnnouncementsColor));
        AddElementToFTXUIContainer(gameWindow, FTXUIDisplayStaringGenerals());

        //Add the input to the gameContainer
        gameWindow->Add(starterGeneralInput);

        //Display what we render AND ALL THE CHANGES
        screen.Loop(renderer);

        PlayerArmies.clear();
    } else {
        //Normal branch
        OutputFTXUIText(beginningGeneralText, gameAnnouncementsColor);
        DisplayStartingGenerals();
        OutputFTXUIText(starterPreChoiceText, userInputExpectedColor);
        std::cin >> ans2;
        sanitizeInputMore(ans2);
        if (ans2 >= StartingGenerals.size()) {
            ans2 = StartingGenerals.size() - 1; //Cap to the last one, negatives also go here
        }
        //Now I can use the starter to show the other classes' functionalities
        Army starterArmy{StartingGenerals[ans2]};
        StartingGenerals.erase(StartingGenerals.begin() + ans2); //Once chosen, gone forever!

        Settlements[0]->StationArmy(std::make_shared<Army>(starterArmy));

        OutputFTXUIText(starterPostChoiceText, importantGameInformationColor);
        Settlements[0]->DisplaySettlement();
        OutputFTXUIText(starterPreTutorial, gameAnnouncementsColor);


        //EXAMPLE TO TEST COMBAT

        //TRYING TO GET A CAPTAIN IN AN ARMY - SUCCESS!

        Settlements[0]->AddUnitToArmy(PlayerGenerals[5]); //Good general
        Settlements[0]->AddUnitToArmy(Captains[Captains.size() - 2]);

        OutputFTXUIText(tutorialFirstDefenceText, storyRelatedTextColor);
        //the first attack doesn't require the attacking army to be actually stationed somewhere,
        //it is scripted and just a one-time occurrence.
        OutputFTXUIText(incomingAttackText, enemyRelatedTextColor);
        InitializeArmiesAndSettlements();
        Settlements[0]->getStationedArmy().value()->DisplayArmy();
        //Settlements[0]->Besieged(warlord1Army);

        //CHECKING IF SETTLEMENT READ IS CORRECT (IT IS)

        for (unsigned long i = 0; i < Settlements.size(); i++) {
            Settlements[i]->DisplaySettlement();
        }

        //op<< checks for unit
        std::cout << *PlayerGenerals[12];
        std::cout << *Captains[2];

        //try to
        //Enemies[0]->AdvanceTurn();
        Enemies[0]->ModifySettlementOwnership(Settlements[0]);
        Enemies[0]->ModifySettlementOwnership(Settlements[0]);
        Enemies[0]->ModifySettlementOwnership(Settlements[0]);
        Enemies[0]->ModifySettlementOwnership(Settlements[0]);

        //testing SizeViolation
        try {
            starterArmy.AddUnit(Captains[0]);
            starterArmy.AddUnit(Captains[0]);
            starterArmy.AddUnit(Captains[0]);
            starterArmy.AddUnit(Captains[0]);
        } catch (const SizeViolation &err) {
            std::cerr << "Size violation - " << err.what() << std::endl;
        }

        //some tests
        std::cout << "\n\nTesting adding units to a settlement\n\n";
        Army army{Captains[3]};
        std::shared_ptr<Army> army222 = std::make_shared<Army>(army);
        Settlements[2]->StationArmy(army222);
        std::cout << "Before:\n";
        Settlements[2]->DisplaySettlement();
        Settlements[2]->getStationedArmy().value()->DisplayArmy();
        army222->AddUnit(Captains[1]);
        std::cout << "\n\nAfter:\n";
        Settlements[2]->DisplaySettlement();
        Settlements[2]->getStationedArmy().value()->DisplayArmy();
        army222->Disband();
        std::cout << "\n\nAfter disband:\n";
        Settlements[2]->DisplaySettlement();
        Settlements[2]->getStationedArmy().value()->DisplayArmy();

        //Temporary ending to the game
        OutputFTXUIText(tutorialFirstDefenceEndText, storyRelatedTextColor);
        std::cout << "\nThe game will end when you press enter.\n";
        std::string temp;
        OutputFTXUIText(enterToContinueText, userInputExpectedColor);
        std::cin.ignore(); //Flush \n from the buffer
        std::getline(std::cin, temp); //Wait until the player wants to continue


        std::cout << "\n\n\n";
        //Testarea cc si op=
        Scout sc1{1};
        Scout sc2{sc1};
        assert((std::cout << "cc: Atributele se copiază corect\n", sc1 == sc2));
        sc2.setViewRange(7);
        assert((std::cout << "cc: Modificarea copiei nu modifică obiectul inițial\n", sc1 != sc2));
        sc1 = sc2;
        assert((std::cout << "op=: Atributele se copiază corect\n", sc1 == sc2));
        sc1.setViewRange(100);
        assert((std::cout << "op=: Modificarea copiei nu modifică obiectul inițial\n", sc1 != sc2));
    }


    return 0;
}

void Game::AddElementToFTXUIContainer(const ftxui::Component &gameFlowWindow, const ftxui::Element &thingToAdd) {
    gameFlowWindow->Add(ftxui::Renderer([thingToAdd] {
        return thingToAdd;
    }));
}


void Game::AddNewLineToFTXUIContainer(const ftxui::Component &gameFlowWindow) {
    gameFlowWindow->Add(ftxui::Renderer([&] {
        return ftxui::paragraph(" ");
    }));
}

void Game::AddMoreLinesToFTXUIContainer(const ftxui::Component &gameFlowWindow) {
    gameFlowWindow->Add(ftxui::Renderer([&] {
        return ftxui::paragraph(" ") | size(HEIGHT, EQUAL, 5);
    }));
}

void Game::AddHorizontalSpaceToFTXUIContainer(const ftxui::Component &gameFlowWindow) {
    gameFlowWindow->Add(ftxui::Renderer([&] {
        return ftxui::paragraph(" ") | size(WIDTH, EQUAL, 5);
    }));
}


Game::~Game() {
    Settlements.clear();
    StartingGenerals.clear();
    PlayerGenerals.clear();
    ContenderGenerals.clear();
    WarlordGenerals.clear();
    EmperorGenerals.clear();
    Captains.clear();
    Enemies.clear();
    LocalLeaders.clear();
}
