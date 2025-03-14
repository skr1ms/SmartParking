#ifndef TARIFF_HPP
#define TARIFF_HPP

#include <string>
#include <iostream>

using namespace std;

// Class for tariffs
class Tariff {
private:
    int id;
    string name;
    string description;
    double price;

public:
    Tariff(int _id, const string& _name, const string& _description, double _price) : id(_id), name(_name), description(_description), price(_price) {}

    // Getters
    int getId() const { return id; }
    string getName() const { return name; }
    string getDescription() const { return description; }
    double getPrice() const { return price; }

    // Overload the output operator
    friend ostream& operator<<(ostream& os, const Tariff& tariff) {
        os << "Tariff: " << tariff.name << " - " << tariff.price << " RUB (" << tariff.description << ")";
        return os;
    }

    // Overload comparison operators
    bool operator<(const Tariff& other) const {
        return price < other.price;
    }

    bool operator>(const Tariff& other) const {
        return price > other.price;
    }
};


#endif // TARIFF_HPP