#ifndef Scout_H
#define Scout_H
#include <iostream>
#include <memory>

//An upgradeable unit that warns the player about incoming forces (gets more precise / gains other uses as it is upgraded)
template<class T>
class Scout {
private:
    int viewRange;
    std::optional<std::weak_ptr<T> > unit;

public:
    explicit Scout(int viewRange_);

    friend std::ostream& operator<<(std::ostream& os, const Scout& scout) {
        os << "This scout has a view range of: " << scout.viewRange << "\n";
        if (scout.unit.has_value() && scout.unit.value().lock()) {
            os << "This is the unit assigned to be a scout: " << scout.unit.value().lock();
        }
        return os;
    }

    //GETTERS / SETTERS (TEMP)
    void setViewRange(int viewRange_);
};

template<class T>
Scout<T>::Scout(int viewRange_) : viewRange(viewRange_) {
}

template<class T>
void Scout<T>::setViewRange(int viewRange_) {
    viewRange = viewRange_;
}


#endif //Scout_H
