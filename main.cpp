#include <iostream>
#include <fstream>
#include <vector>
#include <nlohmann/json.hpp>
#include "header/Texts.h"
#include "header/Settlement.h"

void PopulateGenerals(std::ifstream generalsJson, std::vector<General> &Generals, std::vector<General> &Starters,
                      unsigned int *types, unsigned int *rarity) {
    nlohmann::json data = nlohmann::json::parse(generalsJson);
    for (const auto &i: data) {
        General general{i["firstName"], i["lastName"], i["type"], i["rarity"],
            i["melee"], i["ranged"], i["armour"],
            i["strength"], i["accuracy"], i["dexterity"]
        };
        int currType = i["type"], currRarity = i["rarity"];
        types[currType]++;
        rarity[currRarity]++;

        if (general.getType() != 0) {
            Generals.push_back(general);
        }
        else {
            Starters.push_back(general);
        }

    }
    generalsJson.close();
}

void PopulateSettlements(std::ifstream settlementsJson, std::vector<Settlement> &Settlements) {
    std::vector<ControlPoint> ControlPoints;
    nlohmann::json data = nlohmann::json::parse(settlementsJson);

    for (const auto &i: data) {
        std::vector<int> neighbours;
        if (i["cpCount"] > 0) {
            for (int k = 0; k < i["cpCount"]; k++) {
                Scout scout{i["controlPoints"][k]["scoutViewRange"]};
                ControlPoint controlPoint{scout, i["controlPoints"][k]["name"], i["controlPoints"][k]["cost"]};
                ControlPoints.push_back(controlPoint);
                for (int p = 0; p < i["controlPoints"][k]["connectionNumber"]; p++) {
                    neighbours.push_back(i["controlPoints"][k]["connections"][p]);
                }
            }
        }
        Garrison garrison(i["startingGarrisonLevel"]);
        Settlement settlement{garrison, ControlPoints, i["name"], i["owner"], neighbours};
        Settlements.push_back(settlement);
    }
    settlementsJson.close();
}

void CheckGenerals(const std::vector<General>& Generals, const std::vector<General>& Starters,
    const unsigned int* types, const unsigned int* rarity) {
    std::cout<<"The generals.json file currently holds " << Generals.size() + Starters.size()<<" generals."<<std::endl;
    for (int i = 0; i < 5; i++) {
        std::cout<<"Type "<< i << " : " << types[i] << std::endl;
    }
    for (int i = 0; i < 4; i++) {
        std::cout<<"Rarity "<< i << " : " << rarity[i] << std::endl;
    }
    for (const auto &i: Starters) {
        std::cout<<i<<"\n";
    }
    for (const auto &i: Generals) {
        std::cout<<i<<"\n";
    }
}

void DisplayStartingGenerals(const std::vector<General>& Starters) {
    int k = 0;
    for (const auto &i: Starters) {
        std::cout<<k<<".\n"<<i<<"\n";
        k++;
    }
}

int main() {
    //types[] and rarity[] are for statistics only, could be deleted sometime
    std::vector<General> Generals, StartingGenerals;
    std::vector<Settlement> Settlements;
    unsigned int types[5] = {0}, rarity[4] = {0}, ans2;
    bool ans1;
    std::ifstream generalsJson("generals.json"), settlementsJson("settlements.json");

    if (!generalsJson || !settlementsJson) {
        std::cerr << "File not found." << std::endl;
        return -1;
    }

    PopulateGenerals(std::move(generalsJson), Generals, StartingGenerals, types, rarity);
    generalsJson.close();
    PopulateSettlements(std::move(settlementsJson), Settlements);
    settlementsJson.close();

    std::cout<<welcomeText;
    std::cout<<balanceCheckText;
    std::cin>>ans1;
    if (ans1 == true) {
        std::string temp;
        CheckGenerals(Generals, StartingGenerals, types, rarity);
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
    std::cout << StartingGenerals[ans2] << starterPostChoiceText;

    //Now I can use the starter to show the other classes' functionalities
    Army starterArmy{StartingGenerals[ans2]};
    std::cout << starterArmy << "\n";

    Settlements[0].StationArmy(starterArmy);

    std::cout<<Settlements[0]<<"\n";

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
    Generals.clear();
    return 0;
}
