#ifndef OOP_SABOTEUR_H
#define OOP_SABOTEUR_H
#include <memory>
#include <random>

#include "Captain.h"
#include "Emperor.h"
#include "General.h"
#include "LocalLeader.h"

//Any unit can be a saboteur, and it has different stats depending on the type
template <class T>
class Saboteur {
private:
    std::weak_ptr<T> unit;

public:
    explicit Saboteur(std::weak_ptr<T> unit_);

    //A saboteur can be recruited (constructed) and then used to sabotage armies (kill or debuff a random assigned unit)
    int Sabotage();

    Saboteur(const Saboteur &other) = delete;

    Saboteur(Saboteur &&other) noexcept = delete;

    Saboteur & operator=(const Saboteur &other) = delete;

    Saboteur & operator=(Saboteur &&other) noexcept = delete;
};

template<class T>
Saboteur<T>::Saboteur(std::weak_ptr<T> unit_) : unit(unit_) {}

template<class T>
int Saboteur<T>::Sabotage() {
    bool canKill = false;
    float chanceOfDebuff, chanceOfKilling = 0.f;
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_real_distribution<> distribution(1, 100);

    if (typeid(T) == typeid(Captain)) {
        chanceOfDebuff = 50.0f;
    }
    else if (typeid(T) == typeid(General)) {
        chanceOfDebuff = 80.5f;
        canKill = true;
        chanceOfKilling = 33.3f;
    }
    else if (typeid(T) == typeid(LocalLeader)) {
        chanceOfDebuff = 66.6f;
        canKill = false;
    }
    else if (typeid(T) == typeid(Emperor)) {
        chanceOfDebuff = 100.0f;
        canKill = true;
        chanceOfKilling = 100.0f;
    }
    else if (std::is_base_of_v<Unit, T> == true) {
        std::cout<<"Any other unit that is later introduced\n";
        chanceOfDebuff = 77.7f;
        canKill = false;
    }
    else {
        //a non-unit can't sabotage
        return 0;
    }

    //generating result
    if (canKill == false) {
        if (distribution(generator) <= chanceOfDebuff) {
            //successful sabotage, only debuff
            return 1;
        }
        return 0;
    }
    //can kill
    if (distribution(generator) <= chanceOfKilling) {
        //successful sabotage, killed an enemy
        return 2;
    }
    if (distribution(generator) <= chanceOfDebuff) {
        //failed killing but debuffed an enemy
        return 1;
    }

    //all failed
    return 0;
}




#endif //OOP_SABOTEUR_H
