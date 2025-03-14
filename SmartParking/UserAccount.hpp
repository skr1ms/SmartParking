#ifndef USERACCOUNT_HPP
#define USERACCOUNT_HPP

#include <string>
#include <iostream>
#include <stdexcept>

using namespace std;

// Class for managing user accounts
class UserAccount {
private:
    int id;
    string ownerName;
    double balance;

public:
    UserAccount(int _id, const string& _ownerName, double _balance = 0.0) : id(_id), ownerName(_ownerName), balance(_balance) {}

    // Getters
    int getId() const { return id; }
    string getOwnerName() const { return ownerName; }
    double getBalance() const { return balance; }

    // Method to deposit funds
    void deposit(double amount) {
        if (amount <= 0) {
            throw invalid_argument("Deposit amount must be positive");
        }
        balance += amount;
    }

    // Method to withdraw funds
    bool withdraw(double amount) {
        if (amount <= 0) {
            throw invalid_argument("Withdrawal amount must be positive");
        }
        if (balance < amount) {
            return false;
        }
        balance -= amount;
        return true;
    }

    // Overload operators
    UserAccount& operator+=(double amount) {
        deposit(amount);
        return *this;
    }

    UserAccount& operator-=(double amount) {
        if (!withdraw(amount)) {
            throw runtime_error("Insufficient funds");
        }
        return *this;
    }

    // Overload the output operator
    friend ostream& operator<<(ostream& os, const UserAccount& account) {
        os << "Account " << account.ownerName << ": " << account.balance << " RUB";
        return os;
    }
};

#endif // USERACCOUNT_HPP