#ifndef PARKINGSPOT_HPP
#define PARKINGSPOT_HPP

#include <iostream>

// Structure to store parking spot information
struct ParkingSpot {
    int id, number, vehicleId;
    bool isOccupied;

    ParkingSpot(int _id, int _number, bool _isOccupied, int _vehicleId = 0) : id(_id), number(_number), isOccupied(_isOccupied), vehicleId(_vehicleId) {}
};

#endif // PARKINGSPOT_HPP