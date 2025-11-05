#include "Blockchain.h"
#include <unordered_map>

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

void Blockchain::minePendingTransactions(const string& mining_reward_address, std::vector<User>& users, size_t max_txs_per_block) {
    // Record previous mempool size
    size_t prev_mempool = pending_transactions.size();

    // We'll select up to max_txs_per_block valid transactions from pending_transactions
    std::vector<Transaction> selected;
    selected.reserve(max_txs_per_block + 1); // +1 for reward tx

    // Build a simulated balance map from current users to validate sequentially
    std::unordered_map<string, double> sim_balances;
    for (const auto& user : users) sim_balances[user.getPublicKey()] = user.getBalance();

    size_t rejected_invalid_id = 0;
    size_t rejected_funds = 0;

    for (const auto& tx : pending_transactions) {
        if (selected.size() >= max_txs_per_block) break;

        // Verify transaction ID by recomputing
        std::string txdata = tx.toString();
        std::string recomputed = hash_function(txdata);
        if (recomputed != tx.getTransactionId()) {
            // invalid tx id - skip
            rejected_invalid_id++;
            continue;
        }

        std::string from = tx.getFromAddress();
        std::string to = tx.getToAddress();
        double amount = tx.getAmount();

        if (from == "SYSTEM") {
            // reward-like or system txs are always allowed
            selected.push_back(tx);
            // update simulated balance for receiver
            sim_balances[to] += amount;
            continue;
        }

        double sender_balance = 0.0;
        auto it = sim_balances.find(from);
        if (it != sim_balances.end()) sender_balance = it->second;

        if (sender_balance >= amount) {
            // accept: apply to simulated balances
            sim_balances[from] = sender_balance - amount;
            sim_balances[to] += amount;
            selected.push_back(tx);
        } else {
            // insufficient funds - skip for now
            rejected_funds++;
            continue;
        }
    }

    // Add mining reward transaction (always included)
    Transaction reward_transaction("SYSTEM", mining_reward_address, mining_reward);
    selected.push_back(reward_transaction);

    // Create block from selected valid transactions
    Block new_block(getLatestBlock().getBlockHash(), selected, difficulty);

    // Candidate/time-limited mining parameters (v0.2): try K candidates, each for timeLimitMs
    const int CANDIDATES = 5;
    const uint64_t TIME_LIMIT_MS = 5000; // 5 seconds per candidate
    const uint64_t MAX_ATTEMPTS = 0xFFFFFFFFULL; // effectively unlimited per candidate except time

    std::random_device rd;
    std::mt19937_64 rng(rd());
    std::uniform_int_distribution<uint64_t> seed_dist(0, UINT64_MAX);

    bool found = false;
    uint64_t foundAttempts = 0;
    uint64_t foundElapsed = 0;

    for (int c = 0; c < CANDIDATES; ++c) {
        Block candidate = new_block; // copy
        uint64_t seed = seed_dist(rng);
        candidate.setNonce(seed);

        print_both("Attempting candidate " + std::to_string(c + 1) + " with start nonce " + std::to_string(seed) + "\n");

        uint64_t attemptsDone = 0;
        uint64_t elapsedMs = 0;
        bool ok = candidate.tryMineForDuration(TIME_LIMIT_MS, MAX_ATTEMPTS, attemptsDone, elapsedMs);

        print_both("  Candidate " + std::to_string(c + 1) + " tried " + std::to_string(attemptsDone) + " attempts in " + std::to_string(elapsedMs) + "ms -> " + (ok ? string("FOUND") : string("NOT FOUND")) + "\n");

        if (ok) {
            found = true;
            foundAttempts = attemptsDone;
            foundElapsed = elapsedMs;
            new_block = candidate; // use mined candidate
            break;
        }
    }

    if (!found) {
        print_both("No candidate succeeded within time limits — falling back to full mining for this block (may take longer)\n");
        new_block.mineBlock();
    }

    chain.push_back(new_block);

    // Apply selected transactions to actual users
    std::set<string> participants;
    for (const auto& tx : selected) {
        std::string from = tx.getFromAddress();
        std::string to = tx.getToAddress();
        double amount = tx.getAmount();

        if (from != "SYSTEM") {
            for (auto& user : users) {
                if (user.getPublicKey() == from) {
                    bool ok = user.subtractFromBalance(amount);
                    if (ok) participants.insert(from);
                    break;
                }
            }
        }

        for (auto& user : users) {
            if (user.getPublicKey() == to) {
                user.addToBalance(amount);
                participants.insert(to);
                break;
            }
        }
    }

    // Remove selected transactions from pending_transactions (by transaction id)
    std::set<string> included_ids;
    for (const auto& tx : selected) included_ids.insert(tx.getTransactionId());

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
    if (rejected_invalid_id > 0) print_both("Rejected invalid TX IDs: " + std::to_string(rejected_invalid_id) + "\n");
    if (rejected_funds > 0) print_both("Rejected (insufficient funds): " + std::to_string(rejected_funds) + "\n");
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
