#ifndef Enemy_H
#define Enemy_H

#include <vector>
#include "Settlement.h"

//A class that controls how each enemy acts. It also opens up opportunities to do a lot more.
class Enemy {
private:
    int defaultTurnsToAct, currentTurnsToAct; //how often an enemy will attempt to do something
    int index; //the index it occupies in the vector from Game
    std::string name;
    std::vector<std::shared_ptr<Settlement>> ownedSettlements;


public:
    Enemy(int defaultTurnsToAct_, int currentTurnsToAct_, int index_, std::string name_);

    //function that either deletes or adds a settlement to the enemy
    void ModifySettlementOwnership(const std::shared_ptr<Settlement> &settlement);

    void AdvanceTurn();

    //no pointer because I don't want them to be modifiable
    std::vector<Settlement> getOwnedSettlements() const;

    int getCurrentTurnsToAct() const;

    std::string getName() const;
};


#endif //Enemy_H