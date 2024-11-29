#ifndef ERRORS_HPP
#define ERRORS_HPP

#include <exception>
#include <string>

using namespace std;

class ErrorStudentNotFound : public exception {
protected:
    string message;
public:
    ErrorStudentNotFound(const string& msg) : message(msg) {}

    const char* what() const noexcept override {
        return message.c_str();
    }
};

class ErrorCheckpointNotFound : public exception {
protected:
    string message;
public:
    ErrorCheckpointNotFound(const string& msg) : message(msg) {}

    const char* what() const noexcept override {
        return message.c_str();
    }
};

class ErrorDate : public exception {
protected:
    string message;
public:
    ErrorDate(const string& msg) : message(msg) {}

    const char* what() const noexcept override {
        return message.c_str();
    }
};

#endif