#ifndef Scout_H
#define Scout_H

//An upgradeable unit that warns the player about incoming forces (gets more precise / gains other uses as it is upgraded)
class Scout {
private:
    int viewRange;

public:
    explicit Scout (int viewRange_) : viewRange(viewRange_) {

    }

    Scout (const Scout& other) : viewRange(other.viewRange) {
        //std::cout << "Constructor de copiere pentru Scout\n";
    }

    Scout& operator=(const Scout& other) {
        viewRange = other.viewRange;
        //std::cout << "operator= copiere Scout\n";
        return *this;
    }

    bool operator==(const Scout& other) const {
        if (viewRange == other.viewRange) {
            return true;
        }
        return false;
    }

    bool operator!=(const Scout& other) const {
        if (viewRange == other.viewRange) {
            return false;
        }
        return true;
    }

    ~Scout() {
        //std::cout << "Destructor pentru Scout\n";
    }

    friend std::ostream& operator<<(std::ostream& os, const Scout& scout) {
        os << "This scout has a view range of: " << scout.viewRange << "\n";
        return os;
    }

    //GETTERS / SETTERS (TEMP)
    void setViewRange(int vR) {
        viewRange = vR;
    }

};

#endif //Scout_H