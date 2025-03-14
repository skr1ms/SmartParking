#include "ParkingManager.hpp"

void ParkingManager::loadParkingSpots()
{
    parkingSpots.clear();

    try {
        work txn(conn);
        result result = txn.exec("SELECT id, spot_number, is_occupied, COALESCE(vehicle_id, 0) "
            "FROM parking_spots ORDER BY spot_number");
        txn.commit();

        for (const auto& row : result) {
            int id = row[0].as<int>();
            int number = row[1].as<int>();
            bool isOccupied = row[2].as<bool>();
            int vehicleId = row[3].as<int>();

            parkingSpots.emplace_back(id, number, isOccupied, vehicleId);
        }
    }
    catch (const exception& e) {
        throw ParkingException("Error loading parking spots: " + string(e.what()));
    }
}

void ParkingManager::loadTariffs()
{
    tariffs.clear();

    try {
        work txn(conn);
        result result = txn.exec("SELECT id, name, description, price FROM tariffs ORDER BY price");
        txn.commit();

        for (const auto& row : result) {
            int id = row[0].as<int>();
            string name = row[1].as<string>();
            string description = row[2].as<string>();
            double price = row[3].as<double>();

            tariffs.push_back(make_shared<Tariff>(id, name, description, price));
        }
    }
    catch (const exception& e) {
        throw ParkingException("Error loading tariffs: " + string(e.what()));
    }
}

UserAccount* ParkingManager::findOrCreateUserAccount(const string& ownerName)
{
    try {
        work txn(conn);
        result result = txn.exec("SELECT id, owner_name, balance FROM user_accounts WHERE owner_name = $1", params(ownerName));

        if (result.empty()) {
            // If the account doesn't exist, create a new one
            result = txn.exec("INSERT INTO user_accounts (owner_name, balance) VALUES ($1, 0.0) RETURNING id, owner_name, balance", params(ownerName));
            txn.commit();
        }
        else {
            txn.commit();
        }

        if (!result.empty()) {
            int id = result[0][0].as<int>();
            string name = result[0][1].as<string>();
            double balance = result[0][2].as<double>();

            return new UserAccount(id, name, balance);
        }

        throw ParkingException("Failed to find or create user account");
    }
    catch (const exception& e) {
        throw ParkingException("Error working with user account: " + string(e.what()));
    }
}

void ParkingManager::updateUserBalance(int userId, double newBalance)
{
    try {
        work txn(conn);
        txn.exec("UPDATE user_accounts SET balance = $1 WHERE id = $2", params(newBalance, userId));
        txn.commit();
    }
    catch (const exception& e) {
        throw ParkingException("Error updating user balance: " + string(e.what()));
    }
}

void ParkingManager::addTransaction(int userAccountId, double amount, const string& type, const string& description)
{
    try {
        work txn(conn);
        txn.exec("INSERT INTO transaction_history (user_account_id, amount, transaction_type, description) VALUES ($1, $2, $3, $4)", params(userAccountId, amount, type, description));
        txn.commit();
    }
    catch (const exception& e) {
        throw ParkingException("Error adding transaction: " + string(e.what()));
    }
}

shared_ptr<Vehicle> ParkingManager::createVehicle(const string& licensePlate, const string& brand, const string& model, const string& ownerName, int vehicleTypeId)
{
    try {
        // Check if the license plate already exists
        work checkTxn(conn);
        result checkResult = checkTxn.exec("SELECT COUNT(*) FROM vehicles WHERE license_plate = $1", params(licensePlate));
        checkTxn.commit();

        if (checkResult[0][0].as<int>() > 0) {
            throw ParkingException("Vehicle with this license plate already registered");
        }

        // Add the new vehicle
        work txn(conn);
        result result = txn.exec("INSERT INTO vehicles (license_plate, brand, model, owner_name, vehicle_type_id) VALUES ($1, $2, $3, $4, $5) RETURNING id", params(licensePlate, brand, model, ownerName, vehicleTypeId));
        txn.commit();

        if (result.empty()) {
            throw ParkingException("Failed to create vehicle");
        }

        int id = result[0][0].as<int>();

        if (vehicleTypeId == 1) {
            return make_shared<Car>(id, licensePlate, brand, model, ownerName);
        }
        else {
            return make_shared<Truck>(id, licensePlate, brand, model, ownerName);
        }
    }
    catch (const sql_error& e) {
        throw ParkingException("SQL error while creating vehicle: " + string(e.what()));
    }
    catch (const ParkingException& e) {
        throw;
    }
    catch (const exception& e) {
        throw ParkingException("Error creating vehicle: " + string(e.what()));
    }
}

shared_ptr<Vehicle> ParkingManager::getVehicleByLicensePlate(const string& licensePlate)
{
    try {
        work txn(conn);
        result result = txn.exec("SELECT v.id, v.license_plate, v.brand, v.model, v.owner_name, v.vehicle_type_id FROM vehicles v WHERE v.license_plate = $1", params(licensePlate));
        txn.commit();

        if (result.empty()) {
            throw ParkingException("Vehicle with this license plate not found");
        }

        int id = result[0][0].as<int>();
        string plate = result[0][1].as<string>();
        string brand = result[0][2].as<string>();
        string model = result[0][3].as<string>();
        string ownerName = result[0][4].as<string>();
        int typeId = result[0][5].as<int>();

        if (typeId == 1) {
            return make_shared<Car>(id, plate, brand, model, ownerName);
        }
        else {
            return make_shared<Truck>(id, plate, brand, model, ownerName);
        }
    }
    catch (const ParkingException& e) {
        throw;
    }
    catch (const exception& e) {
        throw ParkingException("Error retrieving vehicle: " + string(e.what()));
    }
}

void ParkingManager::deleteVehicle(const string& licensePlate)
{
    try {
        // Check if the vehicle is currently parked
        work checkTxn(conn);
        result checkResult = checkTxn.exec(
            "SELECT COUNT(*) FROM parking_sessions ps "
            "JOIN vehicles v ON ps.vehicle_id = v.id "
            "WHERE v.license_plate = $1 AND ps.is_active = true",
            params(licensePlate));
        checkTxn.commit();

        if (checkResult[0][0].as<int>() > 0) {
            throw ParkingException("Cannot delete vehicle while it is parked");
        }

        work txn(conn);
        result result = txn.exec("DELETE FROM vehicles WHERE license_plate = $1 RETURNING id", params(licensePlate));
        txn.commit();

        if (result.empty()) {
            throw ParkingException("Vehicle with this license plate not found");
        }
    }
    catch (const ParkingException& e) {
        throw;
    }
    catch (const exception& e) {
        throw ParkingException("Error deleting vehicle: " + string(e.what()));
    }
}

int ParkingManager::findFreeParkingSpot()
{
    for (const auto& spot : parkingSpots) {
        if (!spot.isOccupied) {
            return spot.id;
        }
    }
    throw ParkingException("No free parking spots available");
}

void ParkingManager::occupyParkingSpot(int spotId, int vehicleId)
{
    try {
        work txn(conn);
        txn.exec("UPDATE parking_spots SET is_occupied = true, vehicle_id = $1 WHERE id = $2", params(vehicleId, spotId));
        txn.commit();

        // Update local cache
        for (auto& spot : parkingSpots) {
            if (spot.id == spotId) {
                spot.isOccupied = true;
                spot.vehicleId = vehicleId;
                break;
            }
        }
    }
    catch (const exception& e) {
        throw ParkingException("Error occupying parking spot: " + string(e.what()));
    }
}

void ParkingManager::freeParkingSpot(int spotId)
{
    try {
        work txn(conn);
        txn.exec("UPDATE parking_spots SET is_occupied = false, vehicle_id = NULL WHERE id = $1", spotId);
        txn.commit();

        // Update local cache
        for (auto& spot : parkingSpots) {
            if (spot.id == spotId) {
                spot.isOccupied = false;
                spot.vehicleId = 0;
                break;
            }
        }
    }
    catch (const exception& e) {
        throw ParkingException("Error freeing parking spot: " + string(e.what()));
    }
}

int ParkingManager::createParkingSession(int vehicleId, int spotId, int tariffId, double amount)
{
    try {
        work txn(conn);
        result result = txn.exec("INSERT INTO parking_sessions (vehicle_id, parking_spot_id, tariff_id, payment_amount) VALUES ($1, $2, $3, $4) RETURNING id", params(vehicleId, spotId, tariffId, amount));
        txn.commit();

        if (result.empty()) {
            throw ParkingException("Failed to create parking session");
        }

        return result[0][0].as<int>();
    }
    catch (const exception& e) {
        throw ParkingException("Error creating parking session: " + string(e.what()));
    }
}

bool ParkingManager::endParkingSession(int vehicleId)
{
    try {
        // Find the active session for the vehicle
        work findTxn(conn);
        result findResult = findTxn.exec("SELECT ps.id, ps.parking_spot_id FROM parking_sessions ps WHERE ps.vehicle_id = $1 AND ps.is_active = true", params(vehicleId));
        findTxn.commit();

        if (findResult.empty()) {
            return false;
        }

        int sessionId = findResult[0][0].as<int>();
        int spotId = findResult[0][1].as<int>();

        // End the session
        work updateTxn(conn);
        updateTxn.exec("UPDATE parking_sessions SET is_active = false, exit_time = CURRENT_TIMESTAMP WHERE id = $1", sessionId);
        updateTxn.commit();

        // Free the parking spot
        freeParkingSpot(spotId);

        return true;
    }
    catch (const exception& e) {
        throw ParkingException("Error ending parking session: " + string(e.what()));
    }
}

tuple<string, string, string, string> ParkingManager::getSessionReportInfo(int vehicleId)
{
    try {
        work txn(conn);
        result result = txn.exec(
            "SELECT v.owner_name, v.license_plate, v.brand || ' ' || v.model as vehicle_name, "
            "t.name as tariff_name "
            "FROM parking_sessions ps "
            "JOIN vehicles v ON ps.vehicle_id = v.id "
            "JOIN tariffs t ON ps.tariff_id = t.id "
            "WHERE ps.vehicle_id = $1 "
            "ORDER BY ps.id DESC LIMIT 1",
            params(vehicleId));
        txn.commit();

        if (result.empty()) {
            throw ParkingException("Parking session information not found");
        }

        string ownerName = result[0][0].as<string>();
        string licensePlate = result[0][1].as<string>();
        string vehicleName = result[0][2].as<string>();
        string tariffName = result[0][3].as<string>();

        return make_tuple(ownerName, licensePlate, vehicleName, tariffName);
    }
    catch (const exception& e) {
        throw ParkingException("Error retrieving session report information: " + string(e.what()));
    }
}

ParkingManager::ParkingManager() : conn(CONNECTION_STRING), currentAccount(nullptr)
{
    try {
        if (!conn.is_open()) {
            throw ParkingException("Failed to connect to the database");
        }

        loadParkingSpots();
        loadTariffs();
    }
    catch (const exception& e) {
        throw ParkingException("Error initializing parking manager: " + string(e.what()));
    }
}

void ParkingManager::initializeUserAccount(const string& ownerName)
{
    if (currentAccount) {
        delete currentAccount;
    }

    currentAccount = findOrCreateUserAccount(ownerName);
}

double ParkingManager::getCurrentBalance() const
{
    if (!currentAccount) {
        throw ParkingException("User account not initialized");
    }

    return currentAccount->getBalance();
}

void ParkingManager::depositFunds(double amount)
{
    if (!currentAccount) {
        throw ParkingException("User account not initialized");
    }

    try {
        *currentAccount += amount;
        updateUserBalance(currentAccount->getId(), currentAccount->getBalance());
        addTransaction(currentAccount->getId(), amount, "deposit", "Account deposit");
    }
    catch (const exception& e) {
        throw ParkingException("Error depositing funds: " + string(e.what()));
    }
}

bool ParkingManager::payForTariff(int tariffId, int vehicleTypeId)
{
    if (!currentAccount) {
        throw ParkingException("User account not initialized");
    }

    // Find the selected tariff
    shared_ptr<Tariff> selectedTariff = nullptr;
    for (const auto& tariff : tariffs) {
        if (tariff->getId() == tariffId) {
            selectedTariff = tariff;
            break;
        }
    }

    if (!selectedTariff) {
        throw ParkingException("Tariff not found");
    }

    // Calculate the final price based on vehicle type
    double multiplier = 1.0;
    if (vehicleTypeId == 2) { // Truck
        multiplier = 1.5;
    }

    double finalPrice = selectedTariff->getPrice() * multiplier;

    // Check if the balance is sufficient
    if (currentAccount->getBalance() < finalPrice) {
        return false;
    }

    try {
        *currentAccount -= finalPrice;
        updateUserBalance(currentAccount->getId(), currentAccount->getBalance());
        addTransaction(currentAccount->getId(), finalPrice, "payment",
            "Payment for tariff " + selectedTariff->getName());

        return true;
    }
    catch (const exception& e) {
        throw ParkingException("Error paying for tariff: " + string(e.what()));
    }
}

shared_ptr<Vehicle> ParkingManager::registerVehicle(const string& licensePlate, const string& brand, const string& model, const string& ownerName, int vehicleType)
{
    if (vehicleType != 1 && vehicleType != 2) {
        throw ParkingException("Invalid vehicle type");
    }

    return createVehicle(licensePlate, brand, model, ownerName, vehicleType);
}

void ParkingManager::removeVehicle(const string& licensePlate)
{
    deleteVehicle(licensePlate);
}

void ParkingManager::enterParking(const string& licensePlate, int tariffId)
{
    try {
        // Get vehicle information
        auto vehicle = getVehicleByLicensePlate(licensePlate);

        // Pay for the selected tariff
        if (!payForTariff(tariffId, vehicle->getTypeId())) {
            throw ParkingException("Insufficient funds to pay for the tariff");
        }

        // Find a free parking spot
        int spotId = findFreeParkingSpot();

        // Occupy the parking spot
        occupyParkingSpot(spotId, vehicle->getId());

        // Find the selected tariff for displaying the price
        shared_ptr<Tariff> selectedTariff = nullptr;
        for (const auto& tariff : tariffs) {
            if (tariff->getId() == tariffId) {
                selectedTariff = tariff;
                break;
            }
        }

        if (!selectedTariff) {
            throw ParkingException("Tariff not found");
        }

        // Calculate the final price based on vehicle type
        double multiplier = 1.0;
        if (vehicle->getTypeId() == 2) { // Truck
            multiplier = 1.5;
        }

        double finalPrice = selectedTariff->getPrice() * multiplier;

        // Create a parking session
        createParkingSession(vehicle->getId(), spotId, tariffId, finalPrice);

        // Determine the parking spot number for display
        int spotNumber = 0;
        for (const auto& spot : parkingSpots) {
            if (spot.id == spotId) {
                spotNumber = spot.number;
                break;
            }
        }

        cout << "Vehicle " << licensePlate << " successfully entered the parking lot." << endl;
        cout << "Parking spot: " << spotNumber << endl;
        cout << "Selected tariff: " << selectedTariff->getName() << " (" << finalPrice << " RUB)" << endl;
    }
    catch (const ParkingException& e) {
        throw;
    }
    catch (const exception& e) {
        throw ParkingException("Error entering parking: " + string(e.what()));
    }
}

bool ParkingManager::exitParking(const string& licensePlate, bool generateReport)
{
    try {
        // Get vehicle information
        auto vehicle = getVehicleByLicensePlate(licensePlate);

        // Get session report information if needed
        tuple<string, string, string, string> reportInfo;
        if (generateReport) {
            reportInfo = getSessionReportInfo(vehicle->getId());
        }

        // End the parking session
        bool success = endParkingSession(vehicle->getId());

        if (!success) {
            throw ParkingException("No active parking session found for this vehicle");
        }

        cout << "Vehicle " << licensePlate << " successfully exited the parking lot." << endl;

        // Generate a report if needed
        if (generateReport) {
            string ownerName, vehicleLicense, vehicleName, tariffName;
            tie(ownerName, vehicleLicense, vehicleName, tariffName) = reportInfo;

            string filename = "report_" + vehicleLicense + "_" +
                to_string(time(nullptr)) + ".txt";

            ofstream report(filename);
            if (report.is_open()) {
                report << "==========================================\n";
                report << "          PARKING SESSION REPORT          \n";
                report << "==========================================\n\n";
                report << "Date: " << getCurrentDateTime() << "\n\n";
                report << "Owner: " << ownerName << "\n";
                report << "License Plate: " << vehicleLicense << "\n";
                report << "Vehicle: " << vehicleName << "\n";
                report << "Selected Tariff: " << tariffName << "\n";
                report << "==========================================\n";
                report.close();

                cout << "Report saved to file: " << filename << endl;
            }
            else {
                cout << "Failed to create report file." << endl;
            }
        }

        return true;
    }
    catch (const ParkingException& e) {
        throw;
    }
    catch (const exception& e) {
        throw ParkingException("Error exiting parking: " + string(e.what()));
    }
}

void ParkingManager::displayTariffs() const
{
    cout << "Available Tariffs:" << endl;
    for (const auto& tariff : tariffs) {
        cout << tariff->getId() << ". " << *tariff << endl;
    }
}

void ParkingManager::displayParkingSpots() const
{
    cout << "Parking Spots:" << endl;
    int freeSpotsCount = 0;

    for (const auto& spot : parkingSpots) {
        cout << "Spot #" << spot.number << ": ";
        if (spot.isOccupied) {
            cout << "Occupied" << endl;
        }
        else {
            cout << "Free" << endl;
            freeSpotsCount++;
        }
    }

    cout << "Total free spots: " << freeSpotsCount << " out of " << parkingSpots.size() << endl;
}

string ParkingManager::getCurrentDateTime() const
{
    auto now = time(nullptr);
    tm tm;
    localtime_s(&tm, &now);

    ostringstream oss;
    oss << put_time(&tm, "%d.%m.%Y %H:%M:%S");
    return oss.str();
}

ParkingManager::~ParkingManager()
{
    if (currentAccount) {
        delete currentAccount;
    }
}
