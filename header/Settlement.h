#ifndef Settlement_H
#define Settlement_H
#include <optional>

#include "ControlPoint.h"
#include "Garrison.h"

//The objective of the game, conquering them leads to victory. Has an Army and/or a (weak) Garrison stationed.
//There are Control Points assigned to a Settlement
class Settlement {
private:
    std::optional<Army> stationedArmy;
    Garrison stationedGarrison;
    std::vector<ControlPoint> controlPoints;
    std::string name;
    int owner; //0 = player, others are enemies or contenders
    std::vector<int> neighbours; //the index of neighbouring settlements (in the vector)

public:
    Settlement(const Garrison &garrison_, const std::vector<ControlPoint> &controlPoints_, std::string name_,
               int owner_, std::vector<int> neighbours_) : stationedGarrison(garrison_), controlPoints(controlPoints_),
                                                           name(std::move(name_)),
                                                           owner(owner_), neighbours(std::move(neighbours_)) {
    }

    void StationArmy(const Army& army) {
        stationedArmy = army;
    }

    void Besieged(const Army& attackingArmy) const {
        //If there is a stationedArmy, there will be a combat prompt to the player.
        //If not, then the player will only get the notification of the outcome.
        int result;
        if (stationedArmy) {
             result = stationedArmy->Attacked(attackingArmy, stationedGarrison.GetOverallPower());
        }
        else {
            result = stationedGarrison.DirectlyAttacked(attackingArmy);
        }
        switch (result) {
            case 1: {
                std::cout << this->name << settlementBesiegeFailedText << "\n";
                break;
            }
            case -1: {
                std::cout << this->name << settlementBesiegeSuccessText << "\n";
                break;
            }
            default: {
                std::cerr << "Undefined behaviour detected!" << "\n";
            }
        }
    }

    /*void StationArmyInControlPoint(const Army& army, const int index) {
        controlPoints[index].StationArmy(army);
    }*/

    //There can only be one Army stationed at a time, so I simply remove it.
    /*void MoveArmy(const Army& army) {

        stationedArmy = std::nullopt;
    }*/

    friend std::ostream& operator<<(std::ostream& os, const Settlement& settlement) {
        int k = 0;
        os << "Settlement name: " << settlement.name << "\n";
        if (settlement.owner == 0) {
            os << "Owned by the player\n";
        }
        os << settlement.stationedGarrison;
        os << "List of attached control points:\n";
        for (const auto& i: settlement.controlPoints) {
            os << k << ".\n" << i << "\n";
            k++;
        }
        os<<"\n";
        if (settlement.stationedArmy.has_value()) {
            os << settlement.stationedArmy.value() << "\n";
        }
        return os;
    }
};

#endif