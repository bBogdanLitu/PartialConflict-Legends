#ifndef Enemy_H
#define Enemy_H

#include <vector>
#include "Settlement.h"

//A class that controls how each enemy acts. It also opens up opportunities to do a lot more.
class Enemy {
private:
    bool discovered = false;
    int defaultTurnsToAct, currentTurnsToAct; //how often an enemy will attempt to do something
    int index;
    std::string name;
    std::vector<std::weak_ptr<Settlement> > ownedSettlements;

    void TryToAttack(const ftxui::Component &gameWindow);

public:
    Enemy(int defaultTurnsToAct_, int currentTurnsToAct_, int index_, std::string name_);

    //function that either deletes or adds a settlement to the enemy
    void ModifySettlementOwnership(const std::shared_ptr<Settlement> &settlement);

    void AdvanceTurn(const ftxui::Component &gameWindow);

    bool CheckShouldBeDiscovered();

    void Discovered();

    //no pointer because I don't want them to be modifiable
    [[nodiscard]] std::vector<Settlement> getOwnedSettlements() const;

    [[nodiscard]] bool getDiscovered() const;

    [[nodiscard]] int getCurrentTurnsToAct() const;

    [[nodiscard]] const std::string &getName() const;

    [[nodiscard]] int getIndex() const;
};


#endif //Enemy_H
