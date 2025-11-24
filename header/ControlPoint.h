#ifndef ControlPoint_H
#define ControlPoint_H
#include <optional>
#include <iostream>
#include "Army.h"
#include "Scout.h"

//Intermediary nodes between Settlements that facilitate the Turn-Based concept and warn the player of approaching Armies
//Can have a Scout and/or an Army
class  ControlPoint {
private:
    std::optional<Army> stationedArmy;
    Scout appointedScout;
    std::string name;
    int travelCost; //consume the action points of an army passing through by the cost
    int indexOfOwnerSettlement;
    int indexOfConnectedSettlement;

public:
    ControlPoint(const Scout &appointedScout_, const std::string &name_, int travelCost_, int owner_, int connected_);

    /*void StationArmy(const Army& army) {
        stationedArmy = army;
    }*/

    [[nodiscard]] int getTravelCost() const;

    [[nodiscard]] int getIndexOfConnectedSettlement() const;

    [[nodiscard]] int getIndexOfOwnerSettlement() const;


    friend std::ostream& operator<<(std::ostream& os, const ControlPoint& controlPoint) {
        os << "Point name: " << controlPoint.name << "\n" << controlPoint.appointedScout
        << "The point has a travel cost of: "<< controlPoint.travelCost;
        return os;
    }
};


#endif //ControlPoint_H
