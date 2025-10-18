#include <iostream>
#include <fstream>
#include <vector>
#include <nlohmann/json.hpp>
#include "header/Texts.h"
#include "header/Settlement.h"

void PopulateGenerals(std::vector<General>& Generals, std::vector<General>& Starters, unsigned int* types, unsigned int* rarity ) {
    std::ifstream generalsJson("generals.json");
    nlohmann::json data = nlohmann::json::parse(generalsJson);
    for (auto & i : data) {
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

void CheckGenerals(const std::vector<General>& Generals, const std::vector<General>& Starters,
    const unsigned int* types, const unsigned int* rarity) {
    std::cout<<"The generals.json file currently holds " << Generals.size() + Starters.size()<<" generals."<<std::endl;
    for (int i = 0; i < 5; i++) {
        std::cout<<"Type "<< i << " : " << types[i] << std::endl;
    }
    for (int i = 0; i < 4; i++) {
        std::cout<<"Rarity "<< i << " : " << rarity[i] << std::endl;
    }
    for (auto & i : Starters) {
        std::cout<<i<<"\n";
    }
    for (auto & i : Generals) {
        std::cout<<i<<"\n";
    }
}

void DisplayStartingGenerals(const std::vector<General>& Starters) {
    int k = 0;
    for (auto & i : Starters) {
        std::cout<<k<<".\n"<<i<<"\n";
        k++;
    }
}

int main() {
    //Initialize Generals std::vector and 2 vectors for statistics
    std::vector<General> Generals, StartingGenerals;
    std::string temp;
    unsigned int types[5] = {0}, rarity[4] = {0}, ans2;
    bool ans1;
    PopulateGenerals(Generals, StartingGenerals, types, rarity);

    std::cout<<welcomeText;
    std::cout<<balanceCheckText;
    std::cin>>ans1;
    if (ans1 == true) {
        CheckGenerals(Generals, StartingGenerals, types, rarity);
        std::cout<<"\nHit ENTER when you want to continue!\n";
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
    std::cout<<StartingGenerals[ans2]<<"This is your starter. Good luck!\n";
    std::cout<<starterPostChoiceText;


    //Now I can use the starter to show the other classes' functionalities
    Army starterArmy{StartingGenerals[ans2]};
    std::cout<<starterArmy<<"\n";

    Garrison garrison{1};
    Scout scout1{1};
    ControlPoint controlPoint1{scout1, "Pod", 1}, controlPoint2{scout1, "Giratoriu", 1};

    std::vector<ControlPoint> controlPoints;
    controlPoints.push_back(controlPoint1);
    controlPoints.push_back(controlPoint2);

    Settlement settlement{garrison, controlPoints, "Cernavoda", 0};
    settlement.StationArmy(starterArmy);
    std::cout<<settlement;

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


    controlPoints.clear();
    StartingGenerals.clear();
    Generals.clear();
    return 0;
}
