#include "all_classes.h"
#include "main.h"
#include <random>
#include <set>
#include <chrono>
#include <ctime>

using namespace std;
using namespace std::chrono;

// ---------------------- Transaction (implementation) ----------------------
Transaction::Transaction() : amount(0) {
    timestamp = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    generateTransactionId();
}

Transaction::Transaction(const string& from, const string& to, double amount)
    : from_address(from), to_address(to), amount(amount) {
    timestamp = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    generateTransactionId();
}

string Transaction::getFromAddress() const { return from_address; }
string Transaction::getToAddress() const { return to_address; }
double Transaction::getAmount() const { return amount; }
string Transaction::getTransactionId() const { return transaction_id; }
uint64_t Transaction::getTimestamp() const { return timestamp; }

string Transaction::toString() const {
    return from_address + to_address + std::to_string(amount) + std::to_string(timestamp);
}

void Transaction::generateTransactionId() {
    string data = toString();
    transaction_id = hash_function(data);
}

void Transaction::displayTransaction() const {
    print_both("TX ID: " + transaction_id.substr(0, 16) + "... | " +
               "From: " + from_address.substr(0, 8) + "... -> " +
               "To: " + to_address.substr(0, 8) + "... | " +
               "Amount: " + std::to_string(amount) + "\n");
}

// ---------------------- User (implementation) ----------------------
User::User() : balance(0.0) { generatePublicKey(); }

User::User(const string& name, double initial_balance)
    : name(name), balance(initial_balance) { generatePublicKey(); }

string User::getName() const { return name; }
string User::getPublicKey() const { return public_key; }
double User::getBalance() const { return balance; }

void User::setBalance(double new_balance) { balance = new_balance; }
void User::addToBalance(double amount) { balance += amount; }

bool User::subtractFromBalance(double amount) {
    if (balance >= amount) { balance -= amount; return true; }
    return false;
}

void User::generatePublicKey() {
    string data = name + std::to_string((int)balance);
    public_key = hash_function(data);
}

void User::displayUser() const {
    print_both("User: " + name + " | Public Key: " + public_key.substr(0, 16) +
               "... | Balance: " + std::to_string(balance) + "\n");
}
