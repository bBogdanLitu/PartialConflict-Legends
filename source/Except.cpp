//
// Created by bogdan on 1/7/26.
//

#include "../header/Except.h"

#include "../header/Army.h"

ApplicationException::ApplicationException(const std::string &error) : runtime_error(error) {
}

ObjectFail::ObjectFail(const std::string &type) : ApplicationException{
    "Object of type " + type + " couldn't be created because one of the attributes isn't valid"
} {
}

BrokenFile::BrokenFile(const std::string &fileName) : ApplicationException{
    "The file " + fileName + " wasn't opened. Check your files."
} {
}

SizeViolation::SizeViolation(const Army *army) : ApplicationException{
    "This army has 3 units already! (as you can see above)"
} {
    army->DisplayArmy();
}
