#ifndef MENU_HPP
#define MENU_HPP

#include "ParkingManager.hpp"

// Function to display the main menu
void showMainMenu(ParkingManager& manager) {
    int choice;
    std::string ownerName, licensePlate, brand, model;
    int vehicleType;
    double amount;

    std::cout << "Enter your name: ";
    std::getline(std::cin, ownerName);

    try {
        manager.initializeUserAccount(ownerName);

        while (true) {
            std::cout << "\n===== PARKING MANAGEMENT SYSTEM =====\n";
            std::cout << "Current balance: " << manager.getCurrentBalance() << " RUB\n";
            std::cout << "1. Select a tariff and enter the parking lot\n";
            std::cout << "2. Deposit funds into the virtual wallet\n";
            std::cout << "3. Register a vehicle\n";
            std::cout << "4. Remove vehicle data\n";
            std::cout << "5. View free parking spots\n";
            std::cout << "6. Exit the parking lot\n";
            std::cout << "7. Exit\n";
            std::cout << "Choose an action: ";

            if (!(std::cin >> choice)) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Input error. Please try again.\n";
                continue;
            }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            try {
                switch (choice) {
                case 1: {
                    // Select a tariff and enter the parking lot
                    manager.displayTariffs();
                    std::cout << "Select a tariff (enter the number): ";
                    int tariffChoice;
                    std::cin >> tariffChoice;
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                    std::cout << "Enter the vehicle's license plate: ";
                    std::getline(std::cin, licensePlate);

                    try {
                        manager.enterParking(licensePlate, tariffChoice);
                    }
                    catch (const ParkingException& e) {
                        if (std::string(e.what()).find("Insufficient funds") != std::string::npos) {
                            std::cout << "Insufficient funds. Would you like to deposit funds? (y/n): ";
                            char response;
                            std::cin >> response;
                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                            if (response == 'y' || response == 'Y') {
                                std::cout << "Enter the deposit amount: ";
                                std::cin >> amount;
                                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                                manager.depositFunds(amount);
                                std::cout << "Funds deposited. Balance: " << manager.getCurrentBalance() << " RUB.\n";

                                // Retry entering the parking lot
                                manager.enterParking(licensePlate, tariffChoice);
                            }
                        }
                        else {
                            throw;
                        }
                    }
                    break;
                }
                case 2: {
                    // Deposit funds into the virtual wallet
                    std::cout << "Enter the deposit amount: ";
                    std::cin >> amount;
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                    manager.depositFunds(amount);
                    std::cout << "Funds deposited. New balance: " << manager.getCurrentBalance() << " RUB.\n";
                    break;
                }
                case 3: {
                    // Register a vehicle
                    std::cout << "Select the vehicle type:\n";
                    std::cout << "1. Car\n";
                    std::cout << "2. Truck\n";
                    std::cout << "Your choice: ";
                    std::cin >> vehicleType;
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                    std::cout << "Enter the license plate: ";
                    std::getline(std::cin, licensePlate);

                    std::cout << "Enter the brand: ";
                    std::getline(std::cin, brand);

                    std::cout << "Enter the model: ";
                    std::getline(std::cin, model);

                    auto vehicle = manager.registerVehicle(licensePlate, brand, model, ownerName, vehicleType);
                    std::cout << "Vehicle successfully registered:\n";
                    vehicle->displayInfo(true);
                    break;
                }
                case 4: {
                    // Remove vehicle data
                    std::cout << "Enter the vehicle's license plate to delete: ";
                    std::getline(std::cin, licensePlate);

                    manager.removeVehicle(licensePlate);
                    std::cout << "Vehicle data successfully deleted.\n";
                    break;
                }
                case 5: {
                    // View free parking spots
                    manager.displayParkingSpots();
                    break;
                }
                case 6: {
                    // Exit the parking lot
                    std::cout << "Enter the vehicle's license plate: ";
                    std::getline(std::cin, licensePlate);

                    std::cout << "Generate a parking report? (y/n): ";
                    char reportChoice;
                    std::cin >> reportChoice;
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                    bool generateReport = (reportChoice == 'y' || reportChoice == 'Y');
                    manager.exitParking(licensePlate, generateReport);
                    break;
                }
                case 7: {
                    // Exit the program
                    std::cout << "Thank you for using the system. Goodbye!\n";
                    return;
                }
                default:
                    std::cout << "Invalid choice. Please try again.\n";
                }
            }
            catch (const ParkingException& e) {
                std::cout << "Error: " << e.what() << std::endl;
            }
            catch (const std::exception& e) {
                std::cout << "Unknown error: " << e.what() << std::endl;
            }
        }
    }
    catch (const ParkingException& e) {
        std::cout << "Critical error: " << e.what() << std::endl;
    }
    catch (const std::exception& e) {
        std::cout << "Unknown critical error: " << e.what() << std::endl;
    }
}

#endif // MENU_HPP
