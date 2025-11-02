#include "../header/Scout.h"

Scout::Scout(int viewRange_) : viewRange(viewRange_) {
}

Scout::Scout(const Scout &other) : viewRange(other.viewRange) {
    //std::cout << "Constructor de copiere pentru Scout\n";
}

Scout &Scout::operator=(const Scout &other) {
    viewRange = other.viewRange;
    //std::cout << "operator= copiere Scout\n";
    return *this;
}

bool Scout::operator==(const Scout &other) const {
    if (viewRange == other.viewRange) {
        return true;
    }
    return false;
}

bool Scout::operator!=(const Scout &other) const {
    if (viewRange == other.viewRange) {
        return false;
    }
    return true;
}

Scout::~Scout() {
    //std::cout << "Destructor pentru Scout\n";
}

void Scout::setViewRange(int vR) {
    viewRange = vR;
}
