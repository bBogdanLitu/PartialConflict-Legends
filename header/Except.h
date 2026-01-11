#ifndef EXCEPT_H
#define EXCEPT_H
#include <stdexcept>


class Army;

class ApplicationException : public std::runtime_error {
    using std::runtime_error::runtime_error;

public:
    explicit ApplicationException(const std::string &error);
};

class ObjectFail : public ApplicationException {
public:
    explicit ObjectFail(const std::string &type);
};

class BrokenFile : public ApplicationException {
public:
    explicit BrokenFile(const std::string &fileName);
};

class SizeViolation : public ApplicationException {
public:
    explicit SizeViolation(const Army *army);
};

class InvalidSaveAttempt : public ApplicationException {
public:
    explicit InvalidSaveAttempt(const std::string &reason);
};


#endif //EXCEPT_H
