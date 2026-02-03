#ifndef Scout_H
#define Scout_H
#include <iostream>
#include <memory>
//An upgradeable unit that warns the player about incoming forces (gets more precise / gains other uses as it is upgraded)
class Scout {
private:
    int viewRange;

public:
    explicit Scout(int viewRange_);

    friend std::ostream& operator<<(std::ostream& os, const Scout& scout) {
        os << "This scout has a view range of: " << scout.viewRange << "\n";
        return os;
    }
};


#endif //Scout_H
