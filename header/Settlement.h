#ifndef Settlement_H
#define Settlement_H
#include <optional>
#include <nlohmann/detail/string_utils.hpp>
#include <ftxui/dom/table.hpp>
#include <ftxui/component/component.hpp>
#include "ControlPoint.h"
#include "Garrison.h"


//The objective of the game, conquering them leads to victory. Has an Army and/or a (weak) Garrison stationed.
//There are Control Points assigned to a Settlement
class Settlement {
private:
    std::optional<Army> stationedArmy;
    Garrison stationedGarrison;
    std::vector<ControlPoint> ControlPoints;
    std::string name;
    int owner; //0 = player, others are enemies or contenders
    long int income;
    std::vector<int> neighbours; //the index of neighbouring settlements (in the vector)

public:
    Settlement(const Garrison &garrison_, std::string name_, int owner_, long int income_);

    void StationArmy(const Army &army);

    //Because the settlements are added before the control points by the new logic.
    void AddControlPoint(const ControlPoint &controlPoint);

    //This allows me to know how everything is connected to what
    void AddNeighbour(int neighbourIndex);


    //Because Armies can only be modified when they are in a Settlement! (or directly only for test purposes)
    void AddUnitToArmy(const std::shared_ptr<Unit> &unit);

    void Besieged(const Army &attackingArmy) const;

    void FTXUIBesieged(const Army &attackingArmy, const ftxui::Component &whereToDisplay) const;

    [[nodiscard]] int getOwner() const;

    [[nodiscard]] long int getIncome() const;

    [[nodiscard]] std::optional<Army> getStationedArmy() const {
        if (stationedArmy.has_value()) {
            return stationedArmy.value();
        }
        return std::nullopt;
    }

    /*void StationArmyInControlPoint(const Army& army, const int index) {
        controlPoints[index].StationArmy(army);
    }*/

    //There can only be one Army stationed at a time, so I simply remove it.
    /*void MoveArmy(const Army& army) {

        stationedArmy = std::nullopt;
    }*/

    [[nodiscard]] ftxui::Table CreateSettlementsTable(unsigned long indexInTheSettlementVector) const;

    void DisplaySettlement(unsigned long indexInTheSettlementVector) const;

    [[nodiscard]] ftxui::Element FTXUIDisplaySettlement(unsigned long indexInTheSettlementVector) const;

    friend std::ostream& operator<<(std::ostream& os, const Settlement& settlement) {
        int k = 0;
        os << "Settlement name: " << settlement.name << "\n";
        if (settlement.owner == 0) {
            os << "Owned by the player\n";
        }
        os << settlement.stationedGarrison;
        os << "List of attached control points:\n";
        for (const auto &i: settlement.ControlPoints) {
            os << k << ".\n" << i << "\n";
            k++;
        }
        os << "\n";
        if (settlement.stationedArmy.has_value()) {
            os << "And this is the stationed army. " << settlement.stationedArmy.value() << "\n";
        }
        return os;
    }
};



#endif
