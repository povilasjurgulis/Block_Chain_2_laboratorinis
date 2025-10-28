#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include "main.h"
#include "Block.h"
#include "all_classes.h"

class Blockchain {
private:
    std::vector<Block> chain;
    uint32_t difficulty;
    std::vector<Transaction> pending_transactions;
    double mining_reward;

public:
    // Constructor
    Blockchain(uint32_t difficulty = 4, double reward = 100.0);
    
    // Blockchain operations
    void createGenesisBlock();
    Block getLatestBlock() const;
    void addTransaction(const Transaction& transaction);
    void minePendingTransactions(const string& mining_reward_address, std::vector<User>& users);
    
    // Validation and utility
    bool isChainValid() const;
    double getBalance(const string& address) const;
    void displayChain() const;
    size_t getChainSize() const;
    
    // Getters
    uint32_t getDifficulty() const;
    std::vector<Transaction> getPendingTransactions() const;
    
private:
    string generateGenesisHash() const;
};

#endif
