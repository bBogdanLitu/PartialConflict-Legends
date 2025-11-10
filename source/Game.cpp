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
    for (const auto &i: data) {
        Garrison garrison(i["startingGarrisonLevel"]); //Create the garrison according to config
        Settlement settlement{garrison, i["name"], i["owner"], i["income"]};
        Settlements.push_back(settlement); //Settlement is created and added to this collection
    }
    settlementsJson.close();
}

void Game::PopulateControlPoints(std::ifstream controlPointsJson) {
    nlohmann::json data = nlohmann::json::parse(controlPointsJson);
    for (const auto &i: data) {
        Scout scout{i["scoutViewRange"]};
        ControlPoint controlPoint{scout, i["name"], i["ownedBy"]};

        Settlements[i["ownedBy"]].AddControlPoint(controlPoint);

        //Add the connection to both the Settlements' neighbour list
        Settlements[i["ownedBy"]].AddNeighbour(i["connectedTo"]);
        Settlements[i["connectedTo"]].AddNeighbour(i["ownedBy"]);
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

void Game::DisplayStartingGenerals() const {
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

    auto document = table.Render();
    auto screen =
            Screen::Create(Dimension::Fit(document, /*extend_beyond_screen=*/true));
    //Non-blocking output, I don't want to have to output every single thing using FTXUI and its messy scrolling, maybe later
    Render(screen, document);
    screen.Print();
    std::cout << std::endl;
}

void Game::ResetArmiesActionPoints() const {
    for (const auto &i: Settlements) {
        //If the settlement is owned by the player and it has an army
        if (i.getOwner() == 0 && i.getStationedArmy() != std::nullopt) {
            //We reset that army's action points to the default value
            i.getStationedArmy()->resetActionPoints();
        }
    }
}

void Game::CollectIncomeFromSettlements() {
    for (const auto &i: Settlements) {
        //If the settlement is owned by the player
        if (i.getOwner() == 0) {
            //We add that income (or subtract if it's negative, duh) to the total balance of the player
            sarmale += i.getIncome();
        }
    }
}

//FOR LATER
//It is too much of a headache to try to implement this when I don't even properly populate every Warlord army :)
/*void Game::TickAttackCountdown() {
}*/

void Game::NextTurn() {
    currentTurn++;
    ResetArmiesActionPoints();
    CollectIncomeFromSettlements();
    //TickAttackCountdown();
}

int Game::Start() {
    std::ifstream generalsJson, settlementsJson, controlPointsJson, captainsJson;

    generalsJson.open("generals.json");
    settlementsJson.open("settlements.json");
    controlPointsJson.open("controlPoints.json");
    captainsJson.open("captains.json");

    if (!generalsJson || !settlementsJson || !captainsJson || !controlPointsJson) {
        std::cerr << "File not found." << std::endl;
        return -1;
    }
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

        //to scroll text because it is insanely hard apparently
        float focus_y = 0.5f;
        float step = 0.01f;
        float upperLimit = 1.f;
        float lowerLimit = 0.f;

        auto screen = ScreenInteractive::FitComponent(); //a responsive screen that fits the terminal

        //container where all the feedback is - made scrollable using | focusPositionRelative
        auto gameFlowContainer = Container::Vertical({});

        //container with the game control buttons I want to use
        auto gameStateButtonsContainer = Container::Horizontal({});

        //container to have all things related to game display in it
        auto gameContainer = Container::Horizontal({});

        //container for the contextual buttons that can be used
        auto gameContextualButtonsContainer = Container::Horizontal({});

        //STYLES FOR BUTTONS

        auto nextTurnStyle = ButtonOption::Animated(Color::Default, Color::GrayDark,
                                                    Color::Default, Color::White);

        auto exitStyle = ButtonOption::Animated(Color::Default, Color::Orange1,
                                                Color::Default, Color::Red);

        auto testStyle = ButtonOption::Animated(Color::Default, Color::GrayDark,
                                                    Color::Default, Color::White);

        //FUNCTIONS FOR BUTTONS

        auto onNextTurnButtonClick = [&] {
            NextTurn();
            gameFlowContainer->DetachAllChildren(); //clear last turn output
            AddElementToFTXUIContainer(gameFlowContainer, paragraph("next turn started") | color(beautifulBlue));
            focus_y = upperLimit; //So we see the last thing added
        };

        auto onExitButtonClick = [&] {
            screen.Exit();
        };

        auto onTestButtonClick = [&] {
            AddElementToFTXUIContainer(gameFlowContainer, paragraph("stuff happening " + std::to_string(currentTurn))
                | color(importantGameInformationColor));
        };

        //GAME STATE CONTROL BUTTONS

        auto nextTurnButton = Button("Next turn", onNextTurnButtonClick, nextTurnStyle);
        gameStateButtonsContainer->Add(nextTurnButton);

        auto exitButton = Button("Exit", onExitButtonClick, exitStyle);
        gameStateButtonsContainer->Add(exitButton);

        //Adding all containers to the main one

        gameContainer->Add(gameFlowContainer);
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
                       gameContextualButtonsContainer->Render()
                       | frame
                       | size(HEIGHT, GREATER_THAN, Terminal::Size().dimy / 100.0f * 5),
                       separator(),
                       gameFlowContainer->Render()
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

        /*
        for (int i = 0; i < 1000; i++) {
            //function to add whatever element I want to the flow container (useful for later)
            AddStuffToFTXUIContainer(gameFlowContainer, paragraph("bbbbbbb" + std::to_string(i)));
        }
        */

        //test to add buttons
        auto testButton = Button("Press me!", onTestButtonClick, testStyle);
        gameContextualButtonsContainer->Add(testButton);





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

        Settlements[0].StationArmy(starterArmy);

        OutputFTXUIText(starterPostChoiceText, importantGameInformationColor);
        Settlements[0].DisplaySettlement(0);
        OutputFTXUIText(starterPreTutorial, gameAnnouncementsColor);


        //EXAMPLE TO TEST COMBAT

        //TRYING TO GET A CAPTAIN IN AN ARMY - SUCCESS!

        Settlements[0].AddUnitToArmy(PlayerGenerals[5]); //Good general
        Settlements[0].AddUnitToArmy(Captains[Captains.size() - 2]);
        //Captain to test if every unit can fight with every unit
        Army warlord1Army{Captains[0]}; //Captain to test if every unit can fight with every unit
        warlord1Army.AddUnit(WarlordGenerals[3]); //Medium general to test some of the functionalities
        warlord1Army.AddUnit(WarlordGenerals[68]); //OP general to test if the fight is handled correctly in Army.h
        warlord1Army.useActionPoint(); //temporary to get the Github Actions CHECKS
        OutputFTXUIText(tutorialFirstDefenceText, storyRelatedTextColor);
        //the first attack doesn't require the attacking army to be actually stationed somewhere,
        //it is scripted and just a one-time occurrence.
        OutputFTXUIText(incomingAttackText, enemyRelatedTextColor);
        warlord1Army.DisplayArmy();
        Settlements[0].Besieged(warlord1Army);

        //CHECKING IF SETTLEMENT READ IS CORRECT (IT IS)

        for (unsigned long i = 0; i < Settlements.size(); i++) {
            Settlements[i].DisplaySettlement(i);
        }

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

void Game::AddElementToFTXUIContainer(const ftxui::Component& gameFlowWindow, const ftxui::Element& thingToAdd) {
    gameFlowWindow->Add(ftxui::Renderer([thingToAdd] {
        return thingToAdd;
    }));
}
