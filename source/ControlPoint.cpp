#include "../header/ControlPoint.h"

ControlPoint::ControlPoint(const Scout &appointedScout_, const std::string &name_,
                                  int travelCost_) : appointedScout(appointedScout_), name(name_),
                                                     travelCost(travelCost_) {}