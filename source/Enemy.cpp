#include "../header/Enemy.h"

#include <set>

//The enemy will send one of its armies to attack the player
//The army will be sent to the settlement of the player (following a route)
//In its path, it can be attacked by the player (each turn, it will station somewhere - at least temporarily)
//Can be detected by scouts (a button on the GUI or maybe automatic warnings)
void Enemy::TryToAttack(const ftxui::Component &gameWindow) const {
    if (!ownedSettlements.empty()) {
        std::vector<int> targetIndexes; //LIFO
        bool foundNeighbour = false, foundNeighbourWithArmy = false;
        for (const auto &settlement: ownedSettlements) {
            //We check if this settlement is the one neighbouring the player
            int indexOfPlayerSettlement = settlement->CheckNeighboursOwner(0);
            if (indexOfPlayerSettlement != -1) {
                foundNeighbour = true;
                //If it has a stationed Army, it will send it forward
                if (settlement->getStationedArmy().has_value()) {
                    foundNeighbourWithArmy = true;

                    targetIndexes.push_back(indexOfPlayerSettlement);
                    settlement->SendArmy(settlement->getStationedArmy().value(), targetIndexes, index, gameWindow);

                    //After getting its information, the army will be detached from the settlement and sent forward.
                    settlement->DetachArmy();
                    break;
                }
                //If it doesn't, the army should still reach it, but also reach the settlement neighbouring the player
                targetIndexes.push_back(indexOfPlayerSettlement);
                targetIndexes.push_back(settlement->getIndex());
            }
        }
        //If no settlements are currently neighbours with the player
        if (foundNeighbour == false) {
            return;
        }
        //If the settlements that neighbour the player don't have an army
        if (foundNeighbourWithArmy == false) {
            //something complex involving targetIndexes
        }
    }
}
Enemy::Enemy(const int defaultTurnsToAct_, const int currentTurnsToAct_,
             const int index_, std::string name_) : defaultTurnsToAct(defaultTurnsToAct_),
                                                    currentTurnsToAct(currentTurnsToAct_),
                                                    index(index_),
                                                    name(std::move(name_)) {
}


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

void Enemy::AdvanceTurn(const ftxui::Component &gameWindow) {
    currentTurnsToAct--;
    if (currentTurnsToAct == 0) {
        currentTurnsToAct = defaultTurnsToAct;
        //do stuff
        //only discovered enemies can attack the player
        if (discovered == true) {
            TryToAttack(gameWindow);
        }
    }
}

void Enemy::Discovered() {
    discovered = true;
}

std::vector<Settlement> Enemy::getOwnedSettlements() const {
    std::vector<Settlement> settlements;
    for (unsigned long i = 0; i < ownedSettlements.size(); ++i) {
        settlements.push_back(*ownedSettlements[i]);
    }
    return settlements;
}

bool Enemy::getDiscovered() const {
    return discovered;
}

int Enemy::getCurrentTurnsToAct() const {
    return currentTurnsToAct;
}

const std::string& Enemy::getName() const {
    return name;
}


