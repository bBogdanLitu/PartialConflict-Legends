#ifndef Scout_H
#define Scout_H
#include <iostream>

//An upgradeable unit that warns the player about incoming forces (gets more precise / gains other uses as it is upgraded)
class Scout {
private:
    int viewRange;

public:
    explicit Scout (int viewRange_);

    Scout (const Scout& other);

    Scout& operator=(const Scout& other);

    bool operator==(const Scout& other) const;

    bool operator!=(const Scout& other) const;

    ~Scout();

    friend std::ostream& operator<<(std::ostream& os, const Scout& scout) {
        os << "This scout has a view range of: " << scout.viewRange << "\n";
        return os;
    }

    //GETTERS / SETTERS (TEMP)
    void setViewRange(int vR);
};

#endif //Scout_H