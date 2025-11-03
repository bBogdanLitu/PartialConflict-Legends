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
    std::vector<ControlPoint> ControlPoints;
    nlohmann::json data = nlohmann::json::parse(settlementsJson);

    for (const auto &i: data) {
        std::vector<int> neighbours;
        //If there are control points owned by this settlement
        for (int k = 0; k < i["cpCount"]; k++) {
            Scout scout{i["controlPoints"][k]["scoutViewRange"]}; //We create a scout for each one of them
            ControlPoint controlPoint{scout, i["controlPoints"][k]["name"], i["controlPoints"][k]["cost"]};
            ControlPoints.push_back(controlPoint); //And create the control point that will be added to the settlement
            for (int p = 0; p < i["controlPoints"][k]["connectionNumber"]; p++) {
                //We also take into account the neighbours (written in settlements.json),
                //memorised by their index that they would get in the Settlements std::vector.
                neighbours.push_back(i["controlPoints"][k]["connections"][p]);
            }
        }
        Garrison garrison(i["startingGarrisonLevel"]); //Create the garrison according to config
        Settlement settlement{garrison, ControlPoints, i["name"], i["owner"], neighbours};
        Settlements.push_back(settlement); //Settlement is created and added to this collection
        ControlPoints.clear(); //So that we don't have every settlement controlling every control point !!
    }
    settlementsJson.close();
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

int Game::Start() {
    std::ifstream generalsJson, settlementsJson;

    generalsJson.open("generals.json");
    settlementsJson.open("settlements.json");

    if (!generalsJson || !settlementsJson) {
        std::cerr << "File not found." << std::endl;
        return -1;
    }
    PopulateGenerals(std::move(generalsJson));
    generalsJson.close();
    PopulateSettlements(std::move(settlementsJson));
    settlementsJson.close();

    if (WarlordGenerals.size() < warlordMinimumGenerals) {
        std::cout << warlordCountWarningText;
        return -1;
    }
    if (EmperorGenerals.size() < emperorMinimumGenerals) {
        std::cout << emperorCountWarningText;
        return -1;
    }
    //Actual start of the game after all checks
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
    Settlements[0].DisplaySettlement();
    OutputFTXUIText(starterPreTutorial, gameAnnouncementsColor);


    //EXAMPLE TO TEST COMBAT

    //TRYING TO GET A CAPTAIN IN AN ARMY - SUCCESS!
    Captain captain = {"John", "Pork", 0, 1, 50, 50, 50, 50, 50, 50, 13.1f};
    Captain enemyCaptain = {"Big, Bad", "Wolf", 0, 1, 60, 60, 60, 60, 60, 50, 0.7f};
    std::shared_ptr<Unit> captain1 = captain.clone();
    std::shared_ptr<Unit> captain2 = enemyCaptain.clone();

    Settlements[0].AddUnitToArmy(PlayerGenerals[5]); //Good general
    Settlements[0].AddUnitToArmy(captain1); //Captain to test if any unit can fight with any unit
    Army warlord1Army{captain2}; //Captain to test if any unit can fight with any unit
    warlord1Army.AddUnit(WarlordGenerals[3]); //Medium general to test some of the functionalities
    warlord1Army.AddUnit(WarlordGenerals[68]); //OP general to test if the fight is handled correctly in Army.h
    OutputFTXUIText(tutorialFirstDefenceText, storyRelatedTextColor);
    //the first attack doesn't require the attacking army to be actually stationed somewhere,
    //it is scripted and just a one-time occurrence.
    OutputFTXUIText(incomingAttackText, enemyRelatedTextColor);
    warlord1Army.DisplayArmy();
    Settlements[0].Besieged(warlord1Army);


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

    return 0;
}
