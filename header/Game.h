#ifndef Game_H
#define Game_H
#include <iostream>
#include <vector>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include "Settlement.h"

class Game {
private:
    std::vector<General> StartingGenerals, PlayerGenerals, ContenderGenerals, WarlordGenerals, EmperorGenerals;
    std::vector<Settlement> Settlements;
    unsigned int ans1 = 0, ans2 = 0;

    void PopulateGenerals(std::ifstream generalsJson);

    void PopulateSettlements(std::ifstream settlementsJson);

    void CheckGenerals() const;

    void DisplayStartingGenerals() const;

    void OutputFTXUIText(const std::string& textToOutput, ftxui::Color textColor) const;

public:
    int Start();

    /*std::vector<General> GetStartingGenerals() {
        return StartingGenerals;
    }*/
};

/*void PopulateGenerals(std::ifstream generalsJson, std::vector<General> &Starters, std::vector<General> &Players,
                      std::vector<General> &Contenders,
                      std::vector<General> &Warlords, std::vector<General> &Emperors);

void PopulateSettlements(std::ifstream settlementsJson, std::vector<Settlement> &Settlements);

void CheckGenerals(const std::vector<General> &Starters, const std::vector<General> &Players,
                   const std::vector<General> &Contenders,
                   const std::vector<General> &Warlords, const std::vector<General> &Emperors);

void DisplayStartingGenerals(const std::vector<General>& Starters);

int Start();*/

#endif //Game_H
