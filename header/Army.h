#ifndef Army_H
#define Army_H
#include "General.h"
#include <vector>

//Can hold up to 3 generals, min of 1.
class Army {
private:
    std::vector<General> assignedGenerals;

    //These attributes shouldn't be modified by the constructor, they are initialized here and updated (eventually) through functions
    int actionPoints = 1; //Default
    bool isStationed = true; //All armies must start in a Settlement

public:
    explicit Army(const General &general) {
        if (assignedGenerals.size() < 3) {
            assignedGenerals.push_back(general);
        }
    }

    friend std::ostream& operator<<(std::ostream& os, const Army& army) {
        int k = 0;
        os << "Composition of this army:\n";
        for (const auto &general : army.assignedGenerals) {
            os << k << ".\n" << general;
            k++;
        }
        os << "\nAction points at the start of every turn: " << army.actionPoints << "\n";
        if (army.isStationed == true) {
            os << "The army is stationed.\n";
        }
        else {
            os << "The army isn't stationed.\n";
        }
        return os;
    }
};

#endif