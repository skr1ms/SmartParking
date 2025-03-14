#ifndef PARKINGSESSION_HPP
#define PARKINGSESSION_HPP

#include <ctime>
#include <iostream>

using namespace std;

// Class for managing parking sessions
class ParkingSession {
private:
    int id, vehicleId, parkingSpotId, tariffId;
    time_t entryTime, exitTime;
    bool isActive;
    double paymentAmount;

public:
    ParkingSession(int _id, int _vehicleId, int _parkingSpotId, int _tariffId, time_t _entryTime, double _paymentAmount) : id(_id), vehicleId(_vehicleId), parkingSpotId(_parkingSpotId), tariffId(_tariffId), entryTime(_entryTime), exitTime(0), isActive(true), paymentAmount(_paymentAmount) {}

    // Getters
    int getId() const { return id; }

    int getVehicleId() const { return vehicleId; }

    int getParkingSpotId() const { return parkingSpotId; }

    int getTariffId() const { return tariffId; }

    time_t getEntryTime() const { return entryTime; }

    time_t getExitTime() const { return exitTime; }

    bool getIsActive() const { return isActive; }

    double getPaymentAmount() const { return paymentAmount; }

    // Method to end the parking session
    void endSession() {
        if (isActive) {
            exitTime = time(nullptr);
            isActive = false;
        }
    }
};

#endif // PARKINGSESSION_HPP