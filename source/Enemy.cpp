#include "../header/Enemy.h"

Enemy::Enemy(const int defaultTurnsToAct_, const int currentTurnsToAct_,
    const int index_, std::string name_): defaultTurnsToAct(defaultTurnsToAct_),
                                        currentTurnsToAct(currentTurnsToAct_),
                                        index(index_),
                                        name(std::move(name_)) {}

void Enemy::ModifySettlementOwnership(const std::shared_ptr<Settlement> &settlement) {
    for (unsigned long i = 0; i < ownedSettlements.size(); ++i) {
        if (ownedSettlements[i] == settlement) {
            //If it is found, we should delete it.
            ownedSettlements.erase(ownedSettlements.begin() + i);
            break;
        }
    }
    //If we couldn't find the settlement in the vector, we should add it.
    ownedSettlements.push_back(settlement);
}

void Enemy::AdvanceTurn() {
    currentTurnsToAct--;
    if (currentTurnsToAct == 0) {
        currentTurnsToAct = defaultTurnsToAct;
        //do stuff
    }
}

std::vector<Settlement> Enemy::getOwnedSettlements() const {
    std::vector<Settlement> settlements;
    for (unsigned long i = 0; i < ownedSettlements.size(); ++i) {
        settlements.push_back(*ownedSettlements[i]);
    }
    return settlements;
}

int Enemy::getCurrentTurnsToAct() const {
    return currentTurnsToAct;
}

std::string Enemy::getName() const {
    return name;
}


