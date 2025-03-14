#ifndef VEHICLE_HPP
#define VEHICLE_HPP

#include <string>
#include <iostream>
#include <memory>

using namespace std;

// Base class for vehicles
class Vehicle {
protected:
    int id;
    string licensePlate;
    string brand;
    string model;
    string ownerName;
    int typeId;

public:
    Vehicle(int _id, const string& _licensePlate, const string& _brand,const string& _model, const string& _ownerName, int _typeId) : id(_id), licensePlate(_licensePlate), brand(_brand), model(_model), ownerName(_ownerName), typeId(_typeId) {}

    virtual ~Vehicle() = default;

    // Getters
    int getId() const { return id; }
    string getLicensePlate() const { return licensePlate; }
    string getBrand() const { return brand; }
    string getModel() const { return model; }
    string getOwnerName() const { return ownerName; }
    int getTypeId() const { return typeId; }

    // Virtual method to get the type of vehicle
    virtual string getTypeName() const = 0;

    // Overloaded method to display vehicle information
    virtual void displayInfo() const {
        cout << "License Plate: " << licensePlate << ", Brand: " << brand
            << ", Model: " << model << ", Owner: " << ownerName;
    }

    // Overloaded method with an additional parameter for detailed info
    void displayInfo(bool detailed) const {
        displayInfo();
        if (detailed) {
            cout << ", ID: " << id << ", Type: " << getTypeName();
        }
        cout << endl;
    }
};

// Class for cars
class Car : public Vehicle {
public:
    Car(int _id, const string& _licensePlate, const string& _brand,
        const string& _model, const string& _ownerName)
        : Vehicle(_id, _licensePlate, _brand, _model, _ownerName, 1) {
    }

    string getTypeName() const override {
        return "Car";
    }
};

// Class for trucks
class Truck : public Vehicle {
public:
    Truck(int _id, const string& _licensePlate, const string& _brand,
        const string& _model, const string& _ownerName)
        : Vehicle(_id, _licensePlate, _brand, _model, _ownerName, 2) {
    }

    string getTypeName() const override {
        return "Truck";
    }

    // Override displayInfo to add truck-specific info
    void displayInfo() const override {
        Vehicle::displayInfo();
        cout << " (Truck)";
    }
};

#endif // VEHICLE_HPP