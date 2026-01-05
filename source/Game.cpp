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


void Game::PopulateEnemies(std::ifstream enemiesJson) {
    nlohmann::json data = nlohmann::json::parse(enemiesJson);
    int count = 1; //0 is the player
    for (const auto &i: data) {
        Enemy enemy{i["defaultTurns"], i["currentTurns"], count, i["name"]};
        Enemies.push_back(std::make_shared<Enemy>(enemy));
        count++;
    }
}

void Game::PopulateGenerals(std::ifstream generalsJson) {
    nlohmann::json data = nlohmann::json::parse(generalsJson);
    for (const auto &i: data) {
        General general{
            i["firstName"], i["lastName"], i["type"], i["rarity"],
            i["melee"], i["ranged"], i["armour"],
            i["strength"], i["accuracy"], i["dexterity"]
        };
        switch (general.getType()) {
            case 0: {
                StartingGenerals.push_back(general.clone());
                break;
            }
            case 1: {
                PlayerGenerals.push_back(general.clone());
                break;
            }
            case 2: {
                ContenderGenerals.push_back(general.clone());
                break;
            }
            case 3: {
                WarlordGenerals.push_back(general.clone());
                break;
            }
            case 4: {
                EmperorGenerals.push_back(general.clone());
                break;
            }
            default: {
                std::cerr << "Unhandled type, check your .json! Game will start anyway." << std::endl;
            }
        }
    }
    generalsJson.close();
}

void Game::PopulateSettlements(std::ifstream settlementsJson) {
    nlohmann::json data = nlohmann::json::parse(settlementsJson);
    int count = 0;
    for (const auto &i: data) {
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

void Game::PopulateControlPoints(std::ifstream controlPointsJson) {
    nlohmann::json data = nlohmann::json::parse(controlPointsJson);
    for (const auto &i: data) {
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
        Captain captain{
            i["firstName"], i["lastName"], i["type"], i["rarity"],
            i["melee"], i["ranged"], i["armour"],
            i["strength"], i["accuracy"], i["dexterity"], captainInitialHandicapMultiplier
        };
        Captains.push_back(captain.clone());
    }
    captainsJson.close();
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

ftxui::Table Game::CreateStartingGeneralsTable() const {
    auto Generals = StartingGenerals;
    std::vector<std::string> statsToPrintForEachGeneral;
    std::vector<std::vector<std::string> > tableContent;
    tableContent.push_back(startingGeneralTableHeaders);

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
    if (settlement.getStationedArmy().has_value()) {
        AddElementToFTXUIContainer(whereToDisplay, paragraph("With the stationed army:"));
        AddElementToFTXUIContainer(whereToDisplay,
                                   (*settlement.getStationedArmy())->FTXUIDisplayArmy());
    } else {
        AddElementToFTXUIContainer(whereToDisplay, paragraph("With no stationed army."));
    }
    AddNewLineToFTXUIContainer(whereToDisplay);
}


void Game::ResetArmiesActionPoints() const {
    for (const auto &i: Settlements) {
        //If the settlement is owned by the player and it has an army
        if (i->getOwner() == 0 && i->getStationedArmy() != std::nullopt) {
            //We reset that army's action points to the default value
            (*i->getStationedArmy())->resetActionPoints();
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
}

//ultimate code sausage / a really long piece of spaghetti
int Game::Start() {
    std::ifstream generalsJson, settlementsJson, controlPointsJson, captainsJson, enemiesJson;

    generalsJson.open("generals.json");
    settlementsJson.open("settlements.json");
    controlPointsJson.open("controlPoints.json");
    captainsJson.open("captains.json");
    enemiesJson.open("enemies.json");

    if (!generalsJson || !settlementsJson || !captainsJson || !controlPointsJson || !enemiesJson) {
        std::cerr << "File not found." << std::endl;
        return -1;
    }
    PopulateEnemies(std::move(enemiesJson));
    PopulateGenerals(std::move(generalsJson));
    PopulateSettlements(std::move(settlementsJson));
    PopulateControlPoints(std::move(controlPointsJson));
    PopulateCaptains(std::move(captainsJson));

    if (WarlordGenerals.size() < warlordMinimumGenerals) {
        std::cout << warlordCountWarningText;
        return -1;
    }
    if (EmperorGenerals.size() < emperorMinimumGenerals) {
        std::cout << emperorCountWarningText;
        return -1;
    }

    //Create the initial enemy armies
    Army warlord1Army{Captains[0]}; //Captain to test if every unit can fight with every unit
    warlord1Army.AddUnit(WarlordGenerals[3]); //Medium general to test some of the functionalities
    warlord1Army.AddUnit(WarlordGenerals[68]); //OP general to test if the fight is handled correctly in Army.h

    Settlements[1]->StationArmy(std::make_shared<Army>(warlord1Army));

    //Make the first enemy discovered
    Enemies[0]->Discovered();

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

        //where to store input temporarily
        std::string tempInput;

        //variables that will be used
        unsigned long startingGeneralChosenIndex = 0;
        bool checkSettlementClickedFirstTime = false, checkEnemyIntentsClickedCurrentTurn = false;

        //button variables so I can use them in functions
        Component testButton, checkSettlementsButton, checkEnemyIntentsButton;

        //to scroll text because it is insanely hard apparently
        float focus_y = 0.5f;
        float step = 0.1f;
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

        //STYLES
        auto nextTurnStyle = ButtonOption::Animated(Color::Default, Color::GrayDark,
                                                    Color::Default, Color::White);

        auto exitStyle = ButtonOption::Animated(Color::Default, Color::Orange1,
                                                Color::Default, Color::Red);

        auto testStyle = ButtonOption::Animated(Color::Default, Color::GrayDark,
                                                Color::Default, Color::White);

        auto checkSettlementsStyle = ButtonOption::Animated(Color::Default, beautifulBlue,
                                                            Color::Default, beautifulGreen);

        auto checkEnemyIntentsStyle = ButtonOption::Animated(Color::Default, beautifulOrange,
                                                             Color::Default, beautifulGreen);

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
            AddElementToFTXUIContainer(gameWindow, paragraph("These are your settlements: \n"));
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
                AddElementToFTXUIContainer(gameWindow, paragraph("Wait... there are none! YOU LOST?!"));
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
            //We can only get information about the enemies we have (had) contact with.
            for (const auto &Enemy: Enemies) {
                if (Enemy->getDiscovered() == true) {
                    std::vector<Settlement> enemySettlements = Enemy->getOwnedSettlements();
                    int turnsToAct = Enemy->getCurrentTurnsToAct();
                    std::string name = Enemy->getName();

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

        //GAME STATE CONTROL BUTTONS

        auto nextTurnButton = Button("Next turn", onNextTurnButtonClick, nextTurnStyle);
        gameStateButtonsContainer->Add(nextTurnButton);

        auto exitButton = Button("Exit", onExitButtonClick, exitStyle);
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
                       gameStateButtonsContainer->Render()
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

        //Add contextual buttons
        testButton = Button("Show ALL settlements", onTestButtonClick, testStyle);
        checkSettlementsButton = Button("Check my settlements", onCheckSettlementsButtonClick, checkSettlementsStyle);
        checkEnemyIntentsButton = Button("Check enemy intents", onCheckEnemyIntentButtonClick, checkEnemyIntentsStyle);
        gameContextualButtonsContainer->Add(testButton);

        //Game intro
        AddElementToFTXUIContainer(gameWindow, paragraph(beginningGeneralText) | color(gameAnnouncementsColor));
        AddElementToFTXUIContainer(gameWindow, FTXUIDisplayStaringGenerals());

        //Every time I want to listen to input from the user, I will have to add an input such as this one
        Component starterGeneralInput = Input(&tempInput, starterPreChoiceText, inputOption);

        //because I have to only catch events that are related to input, not mouse hovers, clicks and other stuff,
        //I can only return true on what I am certain I don't want, then return false for anything else.
        starterGeneralInput |= CatchEvent([&](const Event& event) {
            if (event.is_character() && !std::isdigit(event.character()[0])) {
                return true; //it's not a digit, catch it and prevent it from modifying tempInput
            }
            return false; //it's a digit
        });
        //I only want to prevent/repurpose enter, anything else can go through (and to other catchers, eventually)
        starterGeneralInput |= CatchEvent([&](const Event& event) {
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

        //Add the input to the gameContainer
        gameWindow->Add(starterGeneralInput);

        //Display what we render AND ALL THE CHANGES
        screen.Loop(renderer);

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
        warlord1Army.DisplayArmy();
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


Game::~Game() {
    Settlements.clear();
    StartingGenerals.clear();
    PlayerGenerals.clear();
    ContenderGenerals.clear();
    WarlordGenerals.clear();
    EmperorGenerals.clear();
    Captains.clear();
    Enemies.clear();
}
