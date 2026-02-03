#include "../header/ControlPoint.h"

ControlPoint::ControlPoint(const Scout &appointedScout_, const std::string &name_,
                           int travelCost_, int owner_, int connected_) : appointedScout(appointedScout_), name(name_),
                                                                          travelCost(travelCost_),
                                                                          indexOfOwnerSettlement(owner_),
                                                                          indexOfConnectedSettlement(connected_) {
}

int ControlPoint::getTravelCost() const {
    return travelCost;
}

int ControlPoint::getIndexOfConnectedSettlement() const {
    return indexOfConnectedSettlement;
}

int ControlPoint::getIndexOfOwnerSettlement() const {
    return indexOfOwnerSettlement;
}
