#include "Blockchain.h"

Blockchain::Blockchain(uint32_t difficulty, double reward) 
    : difficulty(difficulty), mining_reward(reward) {
    createGenesisBlock();
}

void Blockchain::createGenesisBlock() {
    std::vector<Transaction> genesis_transactions;
    Block genesis_block("0", genesis_transactions, difficulty);
    
    genesis_block.mineBlock();
    
    chain.push_back(genesis_block);
}

Block Blockchain::getLatestBlock() const {
    return chain.back();
}

void Blockchain::addTransaction(const Transaction& transaction) {
    pending_transactions.push_back(transaction);
    // Reduced verbosity - only show for first few transactions
}

void Blockchain::minePendingTransactions(const string& mining_reward_address, std::vector<User>& users) {
    // Record previous mempool size
    size_t prev_mempool = pending_transactions.size();

    // Add mining reward to pending transactions
    Transaction reward_transaction("SYSTEM", mining_reward_address, mining_reward);
    pending_transactions.push_back(reward_transaction);

    // Create block from current pending transactions
    Block new_block(getLatestBlock().getBlockHash(), pending_transactions, difficulty);

    // Mine the block (proof-of-work)
    new_block.mineBlock();

    // Add mined block to chain
    chain.push_back(new_block);

    // Update user balances based on transactions in the block
    std::set<string> participants;
    for (const auto& tx : new_block.getTransactions()) {
        string from = tx.getFromAddress();
        string to = tx.getToAddress();
        double amount = tx.getAmount();

        if (from != "SYSTEM") {
            // find sender and subtract
            for (auto& user : users) {
                if (user.getPublicKey() == from) {
                    user.subtractFromBalance(amount);
                    participants.insert(from);
                    break;
                }
            }
        }

        // find receiver and add
        for (auto& user : users) {
            if (user.getPublicKey() == to) {
                user.addToBalance(amount);
                participants.insert(to);
                break;
            }
        }
    }

    // Remove transactions that were included in the block from pending_transactions
    std::set<string> included_ids;
    for (const auto& tx : new_block.getTransactions()) included_ids.insert(tx.getTransactionId());

    std::vector<Transaction> remaining;
    remaining.reserve(pending_transactions.size());
    for (const auto& tx : pending_transactions) {
        if (included_ids.find(tx.getTransactionId()) == included_ids.end()) {
            remaining.push_back(tx);
        }
    }
    pending_transactions = std::move(remaining);

    // Print summary: balances updated and mempool change
    print_both("\nBalances updated for " + std::to_string(participants.size()) + " senders/receivers\n");
    print_both("Mempool size after block: " + std::to_string(prev_mempool) + " -> " + std::to_string(pending_transactions.size()) + "\n");
}

bool Blockchain::isChainValid() const {
    for (size_t i = 1; i < chain.size(); i++) {
        const Block& current_block = chain[i];
        const Block& previous_block = chain[i - 1];
        
        // Check if current block is valid
        if (!current_block.isValidBlock()) {
            cout << "Invalid block found at index " << i << endl;
            return false;
        }
        
        // Check if current block points to previous block
        if (current_block.getPreviousBlockHash() != previous_block.getBlockHash()) {
            cout << "Chain linkage broken at block " << i << endl;
            return false;
        }
    }
    
    return true;
}

double Blockchain::getBalance(const string& address) const {
    double balance = 0.0;
    
    // Go through all blocks and transactions
    for (const auto& block : chain) {
        for (const auto& transaction : block.getTransactions()) {
            if (transaction.getFromAddress() == address) {
                balance -= transaction.getAmount();
            }
            if (transaction.getToAddress() == address) {
                balance += transaction.getAmount();
            }
        }
    }
    
    return balance;
}

void Blockchain::displayChain() const {
    print_both("\n--- BLOCKCHAIN STRUCTURE ---\n");
    print_both("Chain length: " + std::to_string(chain.size()) + " blocks\n");
    print_both("Difficulty: " + std::to_string(difficulty) + "\n");
    print_both("Mining reward: " + std::to_string(mining_reward) + "\n");
    print_both("Pending transactions: " + std::to_string(pending_transactions.size()) + "\n");
    
    for (size_t i = 0; i < chain.size(); i++) {
        // Print compact block header similar to the assignment sample
        print_both("\nBlock #" + std::to_string(i));
        if (i == 0) print_both(" (Genesis):\n"); else print_both(":\n");
        chain[i].displayHeader();
    }
    
    print_both("----------------------------\n\n");
}

size_t Blockchain::getChainSize() const {
    return chain.size();
}

uint32_t Blockchain::getDifficulty() const {
    return difficulty;
}

std::vector<Transaction> Blockchain::getPendingTransactions() const {
    return pending_transactions;
}

string Blockchain::generateGenesisHash() const {
    string genesis_data = "Genesis Block - Blockchain v0.1";
    return hash_function(genesis_data);
}
