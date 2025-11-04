#include "all_classes.h"
#include "main.h"
#include <random>
#include <set>
#include <chrono>
#include <ctime>

using namespace std;
using namespace std::chrono;


// ---------------------- User ----------------------
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


// ---------------------- UserGenerator ----------------------
mt19937 UserGenerator::rng;
std::uniform_int_distribution<int> UserGenerator::balance_dist(100, 1000000);

void UserGenerator::initializeRandom() {
    random_device rd; rng.seed(rd());
}

vector<string> UserGenerator::generateRandomNames(int count) {
    vector<string> names; names.reserve(count);
    for (int i = 1; i <= count; i++) names.push_back("User" + std::to_string(i));
    return names;
}

vector<User> UserGenerator::generateUsers(int user_count) {
    initializeRandom();
    vector<User> users; users.reserve(user_count);
    auto names = generateRandomNames(user_count);
    for (int i = 0; i < user_count; i++) {
        double balance = balance_dist(rng);
        User user(names[i], balance);
        users.push_back(user);
    }
    return users;
}

void UserGenerator::displayUserStats(const vector<User>& users) {
    if (users.empty()) return;
    double total_balance = 0.0;
    double min_balance = users[0].getBalance();
    double max_balance = users[0].getBalance();
    for (const auto& user : users) {
        double balance = user.getBalance();
        total_balance += balance;
        min_balance = std::min(min_balance, balance);
        max_balance = std::max(max_balance, balance);
    }
    print_both("User Statistics:\n");
    print_both("  Total Users: " + std::to_string(users.size()) + "\n");
    print_both("  Total Balance: " + std::to_string((long long)total_balance) + "\n");
    print_both("  Average Balance: " + std::to_string((long long)(total_balance / users.size())) + "\n");
    print_both("  Balance Range: " + std::to_string((long long)min_balance) + " - " +
               std::to_string((long long)max_balance) + "\n");
}


// ---------------------- Transaction ----------------------
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


// ---------------------- TransactionGenerator ----------------------
mt19937 TransactionGenerator::rng;
std::uniform_real_distribution<double> TransactionGenerator::amount_dist(1.0, 100.0);

void TransactionGenerator::initializeRandom() {
    random_device rd; rng.seed(rd());
}

vector<Transaction> TransactionGenerator::generateTransactions(const vector<User>& users, int transaction_count) {
    initializeRandom();
    vector<Transaction> transactions; transactions.reserve(transaction_count);
    uniform_int_distribution<size_t> user_dist(0, users.size() - 1);
    int valid_transactions = 0; int attempts = 0;
    while (valid_transactions < transaction_count && attempts < transaction_count * 2) {
        attempts++;
        size_t sender_idx = user_dist(rng);
        size_t receiver_idx = user_dist(rng);
        if (sender_idx == receiver_idx) continue;
        const User& sender = users[sender_idx];
        const User& receiver = users[receiver_idx];
        double max_amount = std::min(sender.getBalance() * 0.1, 100.0);
        if (max_amount < 1.0) continue;
        uniform_real_distribution<double> tx_amount_dist(1.0, max_amount);
        double amount = tx_amount_dist(rng);
        Transaction tx(sender.getPublicKey(), receiver.getPublicKey(), amount);
        transactions.push_back(tx);
        valid_transactions++;
    }
    return transactions;
}

void TransactionGenerator::displayTransactionStats(const vector<Transaction>& transactions) {
    if (transactions.empty()) return;
    double total_amount = 0.0;
    set<string> participants;
    for (const auto& tx : transactions) {
        total_amount += tx.getAmount();
        participants.insert(tx.getFromAddress());
        participants.insert(tx.getToAddress());
    }
    print_both("Transaction Statistics:\n");
    print_both("  Total Transactions: " + std::to_string(transactions.size()) + "\n");
    print_both("  Total Volume: " + std::to_string((int)total_amount) + "\n");
    print_both("  Average Amount: " + std::to_string((int)(total_amount / transactions.size())) + "\n");
    print_both("  Unique Participants: " + std::to_string(participants.size()) + "\n");
}

bool TransactionGenerator::isValidTransaction(const Transaction& tx, const vector<User>& users) {
    for (const auto& user : users) {
        if (user.getPublicKey() == tx.getFromAddress()) {
            return user.getBalance() >= tx.getAmount();
        }
    }
    return false;
}


// ---------------------- MerkleTree ----------------------
string MerkleTree::computePairHash(const string& left, const string& right) {
    string combined = left + right;
    return hash_function(combined);
}

string MerkleTree::computeMerkleRoot(const vector<Transaction>& transactions) {
    if (transactions.empty()) {
        string empty = "";
        return hash_function(empty);
    }
    vector<string> hashes;
    for (const auto& tx : transactions) hashes.push_back(tx.getTransactionId());
    while (hashes.size() > 1) {
        vector<string> next_level;
        for (size_t i = 0; i < hashes.size(); i += 2) {
            if (i + 1 < hashes.size()) next_level.push_back(computePairHash(hashes[i], hashes[i + 1]));
            else next_level.push_back(computePairHash(hashes[i], hashes[i]));
        }
        hashes = next_level;
    }
    return hashes[0];
}
