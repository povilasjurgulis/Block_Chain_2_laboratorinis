#ifndef BLOCK_H
#define BLOCK_H

#include "main.h"
#include "all_classes.h"

class Block {
private:
    // Block Header
    string previous_block_hash;
    uint64_t timestamp;
    string version;
    string merkle_root_hash;
    uint64_t nonce;
    uint32_t difficulty_target;
    
    // Block Body
    std::vector<Transaction> transactions;
    
    // Block properties
    string block_hash;
    bool is_mined;

public:
    // Constructors
    Block();
    Block(const string& prev_hash, const std::vector<Transaction>& txs, uint32_t difficulty = 4);
    
    // Getters
    string getPreviousBlockHash() const;
    uint64_t getTimestamp() const;
    string getVersion() const;
    string getMerkleRootHash() const;
    uint64_t getNonce() const;
    uint32_t getDifficultyTarget() const;
    std::vector<Transaction> getTransactions() const;
    string getBlockHash() const;
    bool getIsMined() const;
    
    // Mining and utility methods
    void mineBlock();
    string calculateBlockHash() const;
    bool isValidBlock() const;
    void displayBlock() const;
    // Compact header display used by Blockchain::displayChain()
    void displayHeader() const;
    
private:
    void computeMerkleRoot();
    string getHeaderString() const;
};

#endif
