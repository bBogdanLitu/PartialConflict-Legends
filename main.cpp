#include <iostream>
#include <fstream>
#include <vector>
#include <nlohmann/json.hpp>
#include "header/Constants.h"
#include "header/Settlement.h"

void PopulateGenerals(std::ifstream generalsJson, std::vector<General> &Starters, std::vector<General> &Players,
                      std::vector<General> &Contenders,
                      std::vector<General> &Warlords, std::vector<General> &Emperors) {
    nlohmann::json data = nlohmann::json::parse(generalsJson);
    for (const auto &i: data) {
        General general{i["firstName"], i["lastName"], i["type"], i["rarity"],
            i["melee"], i["ranged"], i["armour"],
            i["strength"], i["accuracy"], i["dexterity"]
        };
        switch (general.getType()) {
            case 0: {
                Starters.push_back(general);
                break;
            }
            case 1: {
                Players.push_back(general);
                break;
            }
            case 2: {
                Contenders.push_back(general);
                break;
            }
            case 3: {
                Warlords.push_back(general);
                break;
            }
            case 4: {
                Emperors.push_back(general);
                break;
            }
            default: {
                std::cerr << "Unhandled type, check your .json! Game will start anyway." << std::endl;
            }

        }

    }
    generalsJson.close();
}

void PopulateSettlements(std::ifstream settlementsJson, std::vector<Settlement> &Settlements) {
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

void CheckGenerals(const std::vector<General> &Starters, const std::vector<General> &Players,
                   const std::vector<General> &Contenders,
                   const std::vector<General> &Warlords, const std::vector<General> &Emperors) {
    std::cout << "The generals.json file currently holds " << Starters.size() + Players.size() +
            Contenders.size() + Warlords.size() + Emperors.size() << " generals." << std::endl;
    std::cout << "Starter generals for the player: " << Starters.size() << std::endl;
    std::cout << "Generals selectable by the player (at any point): " << Starters.size() + Players.size() << std::endl;
    std::cout << "Contender general pool (doesn't use more than 3): " << Contenders.size() << std::endl;
    std::cout << "Warlord general pool: " << Warlords.size() << std::endl;
    std::cout << "Emperor general pool: " << Emperors.size() << std::endl;
}

void DisplayStartingGenerals(const std::vector<General>& Starters) {
    int k = 0;
    for (const auto &i: Starters) {
        std::cout << k << ".\n" << i << "\n";
        k++;
    }
}

int main() {
    std::vector<General> StartingGenerals, PlayerGenerals, ContenderGenerals, WarlordGenerals, EmperorGenerals;
    std::vector<Settlement> Settlements;
    unsigned int ans2;
    bool ans1;
    std::ifstream generalsJson("generals.json"), settlementsJson("settlements.json");

    if (!generalsJson || !settlementsJson) {
        std::cerr << "File not found." << std::endl;
        return -1;
    }

    PopulateGenerals(std::move(generalsJson), StartingGenerals, PlayerGenerals, ContenderGenerals, WarlordGenerals, EmperorGenerals);
    generalsJson.close();
    PopulateSettlements(std::move(settlementsJson), Settlements);
    settlementsJson.close();

    if (WarlordGenerals.size() < warlordMinimumGenerals) {
        std::cout << warlordCountWarningText;
        return -1;
    }
    if (EmperorGenerals.size() < emperorMinimumGenerals) {
        std::cout<<emperorCountWarningText;
        return -1;
    }

    std::cout<<welcomeText;
    std::cout<<balanceCheckText;
    std::cin >> ans1;
    if (ans1 == true) {
        std::string temp;
        CheckGenerals(StartingGenerals, PlayerGenerals, ContenderGenerals, WarlordGenerals, EmperorGenerals);
        std::cout << enterToContinueText;
        std::cin.ignore(); //Flush \n from the buffer
        std::getline(std::cin, temp); //Wait until the player has read the list / wants to continue
    }
    std::cout<<beginningGeneralText;
    DisplayStartingGenerals(StartingGenerals);
    std::cout<<starterPreChoiceText;
    std::cin>>ans2;
    if (ans2 >= StartingGenerals.size() ) {
        ans2 = StartingGenerals.size() - 1; //Cap to the last one, negatives also go here
    }
    //Now I can use the starter to show the other classes' functionalities
    Army starterArmy{StartingGenerals[ans2]};
    StartingGenerals.erase(StartingGenerals.begin() + ans2); //Once chosen, gone forever!

    Settlements[0].StationArmy(starterArmy);
    std::cout<<starterPostChoiceText;
    std::cout<<Settlements[0];
    std::cout<<starterPreTutorial;


    //EXAMPLE TO TEST COMBAT
    Army warlord1Army{WarlordGenerals[1]};

    std::cout<<tutorialFirstDefenceText;
    //the first attack doesn't require the attacking army to be actually stationed somewhere,
    //it is scripted and just a one-time occurrence.
    Settlements[0].Besieged(warlord1Army);



    std::cout<<"\n\n\n";
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

    Settlements.clear();
    StartingGenerals.clear();
    PlayerGenerals.clear();
    ContenderGenerals.clear();
    WarlordGenerals.clear();
    EmperorGenerals.clear();
    return 0;
}
