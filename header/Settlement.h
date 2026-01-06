#ifndef Settlement_H
#define Settlement_H
#include <optional>
#include <nlohmann/detail/string_utils.hpp>
#include <ftxui/dom/table.hpp>
#include <ftxui/component/component.hpp>
#include "ControlPoint.h"
#include "Garrison.h"


class Enemy;
//The objective of the game, conquering them leads to victory. Has an Army and/or a (weak) Garrison stationed.
//There are Control Points assigned to a Settlement
class Settlement {
private:
    std::optional<std::shared_ptr<Army> > stationedArmy;
    std::optional<std::shared_ptr<Army> > temporaryArmy;
    Garrison stationedGarrison;
    std::vector<ControlPoint> ControlPoints;
    std::string name;
    int owner; //0 = player, others are enemies or contenders
    int index; //index in the vector from Game
    long int income;
    std::vector<std::weak_ptr<Settlement> > Neighbours; //the neighbouring settlements
    std::weak_ptr<Settlement> weakSelfPtr;

public:
    Settlement(const Garrison &garrison_, std::string name_, int owner_, int index_, long int income_);

    void StationArmy(const std::shared_ptr<Army> &army);

    void StationTemporaryArmy(const std::shared_ptr<Army> &army);

    void SendArmy(const std::shared_ptr<Army> &, std::vector<int>, Enemy *, const ftxui::Component &);

    static void AttackAndAnalyzeResult(const std::shared_ptr<Settlement> &, const std::shared_ptr<Army> &, Enemy *,
                                       const ftxui::Component &);

    void DetachArmy();

    void DetachTemporaryArmy();

    [[nodiscard]] int CheckNeighboursOwner(int) const;

    //Because the settlements are added before the control points by the new logic.
    void AddControlPoint(const ControlPoint &controlPoint);

    //This allows me to know how everything is connected to what
    void AddNeighbour(const std::shared_ptr<Settlement> &neighbour);


    //Because Armies can only be modified when they are in a Settlement! (or directly only for test purposes)
    void AddUnitToArmy(const std::shared_ptr<Unit> &unit);

    [[nodiscard]] int Besieged(const Army &attackingArmy, const ftxui::Component &gameWindow) const;

    //gives / removes the ownership of this settlement to / from an enemy
    void ChangeOwnership(Enemy *newOwner);

    //gives the settlement to the player
    void GiveToPlayer(const ftxui::Component &);

    //void FTXUIBesieged(const Army &attackingArmy, const ftxui::Component &whereToDisplay) const;

    void setSelfPtr(const std::shared_ptr<Settlement> &settlement);

    [[nodiscard]] int getOwner() const;

    [[nodiscard]] long int getIncome() const;

    [[nodiscard]] int getIndex() const;

    [[nodiscard]] std::string getName() const;

    [[nodiscard]] std::optional<std::shared_ptr<Army> > getStationedArmy() const;

    [[nodiscard]] std::vector<std::shared_ptr<Settlement> > getNeighbours() const;

    /*void StationArmyInControlPoint(const Army& army, const int index) {
        controlPoints[index].StationArmy(army);
    }*/

    //There can only be one Army stationed at a time, so I simply remove it.
    /*void MoveArmy(const Army& army) {

        stationedArmy = std::nullopt;
    }*/

    [[nodiscard]] ftxui::Table CreateSettlementsTable() const;

    void DisplaySettlement() const;

    [[nodiscard]] ftxui::Element FTXUIDisplaySettlement() const;

    Settlement(const Settlement &other);

    Settlement &operator=(Settlement other);

    friend void swap(Settlement &first, Settlement &second);

    ~Settlement();

    //2 settlements are equal if they occupy the same index in the vector from Game
    friend bool operator==(const Settlement &lhs, const Settlement &rhs) {
        return lhs.index == rhs.index;
    }

    friend bool operator!=(const Settlement &lhs, const Settlement &rhs) {
        return !(lhs == rhs);
    }

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
