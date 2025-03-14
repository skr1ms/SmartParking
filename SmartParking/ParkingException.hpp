#ifndef PARKINGEXCEPTION_HPP
#define PARKINGEXCEPTION_HPP

#include <stdexcept>
#include <string>

using namespace std;

// Custom exception class for parking-related errors
class ParkingException : public exception {
private:
    string message;

public:
    ParkingException(const string& msg) : message(msg) {}

    const char* what() const noexcept override {
        return message.c_str();
    }
};

#endif // PARKINGEXCEPTION_HPP