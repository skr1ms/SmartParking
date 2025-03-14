#ifndef PARKINGMANAGER_HPP
#define PARKINGMANAGER_HPP

#include <pqxx/pqxx>
#include <vector>
#include <fstream>
#include <memory>
#include <iomanip>
#include "Vehicle.hpp"
#include "Tariff.hpp"
#include "UserAccount.hpp"
#include "ParkingSession.hpp"
#include "ParkingException.hpp"
#include "ParkingSpot.hpp"

using namespace std;
using namespace pqxx;

// Connection string for PostgreSQL
const string CONNECTION_STRING = "dbname=SmartParking user=postgres password=564789s port=5432";

class ParkingManager {
private:
    pqxx::connection conn;
    UserAccount* currentAccount;
    vector<ParkingSpot> parkingSpots;
    vector<shared_ptr<Tariff>> tariffs;

    void loadParkingSpots();

    void loadTariffs();

    UserAccount* findOrCreateUserAccount(const string& ownerName);

    void updateUserBalance(int userId, double newBalance);

    void addTransaction(int userAccountId, double amount, const string& type, const string& description);

    shared_ptr<Vehicle> createVehicle(const string& licensePlate, const string& brand,const string& model, const string& ownerName, int vehicleTypeId);

    shared_ptr<Vehicle> getVehicleByLicensePlate(const string& licensePlate);

    void deleteVehicle(const string& licensePlate);

    int findFreeParkingSpot();

    void occupyParkingSpot(int spotId, int vehicleId);

    void freeParkingSpot(int spotId);

    int createParkingSession(int vehicleId, int spotId, int tariffId, double amount);

    bool endParkingSession(int vehicleId);

    tuple<string, string, string, string> getSessionReportInfo(int vehicleId);

public:

    ParkingManager();

    void initializeUserAccount(const string& ownerName);

    double getCurrentBalance() const;

    void depositFunds(double amount);

    bool payForTariff(int tariffId, int vehicleTypeId);

    shared_ptr<Vehicle> registerVehicle(const string& licensePlate, const string& brand, const string& model, const string& ownerName, int vehicleType);

    void removeVehicle(const string& licensePlate);

    void enterParking(const string& licensePlate, int tariffId);

    bool exitParking(const string& licensePlate, bool generateReport = false);

    void displayTariffs() const;

    void displayParkingSpots() const;

    string getCurrentDateTime() const;

    ~ParkingManager();
};

#endif // PARKINGMANAGER_HPP