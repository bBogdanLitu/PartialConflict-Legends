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
    std::vector<int> neighbours;

public:
    Settlement(const Garrison& garrison_,const std::vector <ControlPoint>& controlPoints_, std::string  name_,
        int owner_, std::vector<int> neighbours_) :
            stationedGarrison(garrison_), controlPoints(controlPoints_), name(std::move(name_)),
                owner(owner_), neighbours(std::move(neighbours_)) {

    }

    void StationArmy(const Army& army) {
        stationedArmy = army;
    }

    friend std::ostream& operator<<(std::ostream& os, const Settlement& settlement) {
        int k = 0;
        os << "Settlement name: " << settlement.name << "\n";
        if (settlement.owner == 0) {
            os << "Owned by the player\n";
        }
        if (settlement.stationedArmy.has_value()) {
            os << settlement.stationedArmy.value() << "\n";
        }
        os << settlement.stationedGarrison;
        os << "List of attached control points:\n";
        for (const auto& i: settlement.controlPoints) {
            os << k << ".\n" << i << "\n";
            k++;
        }
        return os;
    }
};

#endif