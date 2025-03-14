#include "Menu.hpp"

int main()
{
    setlocale(LC_ALL, "Russian");
    system("chcp 1251");

    std::cout << "=== PARKING MANAGEMENT SYSTEM ===\n\n";

    try {
        ParkingManager manager;
        showMainMenu(manager);
    }
    catch (const ParkingException& e) {
        std::cout << "Critical initialization error: " << e.what() << std::endl;
        return 1;
    }
    catch (const std::exception& e) {
        std::cout << "Unknown critical error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
