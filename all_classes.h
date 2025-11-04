#ifndef ALL_CLASSES_H
#define ALL_CLASSES_H

// Consolidated declarations for Transaction, User, UserGenerator,
// TransactionGenerator, and MerkleTree. Implementations are in all_classes.cpp.

#include "main.h"
#include <vector>
#include <string>
#include <cstdint>

// User
class User {
private:
    std::string name;
    std::string public_key;
    double balance;

public:
    User();
    User(const std::string& name, double initial_balance);

    std::string getName() const;
    std::string getPublicKey() const;
    double getBalance() const;

    void setBalance(double new_balance);
    void addToBalance(double amount);
    bool subtractFromBalance(double amount);

    void generatePublicKey();
    void displayUser() const;
};

// UserGenerator
class UserGenerator {
private:
    static std::mt19937 rng;
    static std::uniform_int_distribution<int> balance_dist;
    static std::vector<std::string> generateRandomNames(int count);

public:
    static void initializeRandom();
    static std::vector<User> generateUsers(int user_count = 1000);
    static void displayUserStats(const std::vector<User>& users);
};

// Transaction
class Transaction {
private:
    std::string from_address;
    std::string to_address;
    double amount;
    std::string transaction_id;
    uint64_t timestamp;

public:
    Transaction();
    Transaction(const std::string& from, const std::string& to, double amount);

    std::string getFromAddress() const;
    std::string getToAddress() const;
    double getAmount() const;
    std::string getTransactionId() const;
    uint64_t getTimestamp() const;

    std::string toString() const;
    void generateTransactionId();
    void displayTransaction() const;
};

// TransactionGenerator
class TransactionGenerator {
private:
    static std::mt19937 rng;
    static std::uniform_real_distribution<double> amount_dist;

public:
    static void initializeRandom();
    static std::vector<Transaction> generateTransactions(const std::vector<User>& users, int transaction_count = 10000);
    static void displayTransactionStats(const std::vector<Transaction>& transactions);
    static bool isValidTransaction(const Transaction& tx, const std::vector<User>& users);
};

// MerkleTree
class MerkleTree {
private:
    static std::string computePairHash(const std::string& left, const std::string& right);

public:
    static std::string computeMerkleRoot(const std::vector<Transaction>& transactions);
};

#endif // ALL_CLASSES_H
