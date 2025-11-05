#include <iostream>
#include "main.h"
#include "all_classes.h"
#include "Blockchain.h"
#include <chrono>
#include <fstream>

// Povilo Jurgulio VU ISI Blokų Grandinių Technologijų 2 laboratorinis darbas

using namespace std;
using namespace std::chrono;

// Global output file
ofstream output_file;

// Helper function to print to both console and file
void print_both(const string& message) {
    cout << message;
    if (output_file.is_open()) {
        output_file << message;
    }
}

void demonstrateBlockchainProcess(std::vector<User>& users, const std::vector<Transaction>& all_transactions);

int main()
{
    output_file.open("rezultatai.txt");
    if (!output_file.is_open()) {
        cerr << "Error: could not open rezultatai.txt for writing!!!" << endl;
        return 1;
    }
    
    print_both("-----------------------------------------\n");
    print_both("Bloku grandiniu technologiju 2 laboratorinis:\n");
    print_both("------------------------------------------\n\n");
    
    // Step 1: Generate around 1000 users
    print_both("STEP 1: User generation (around 1000 users)\n");
    print_both("----------------------------------------\n");
    auto start_time = high_resolution_clock::now();
    
    std::vector<User> users = UserGenerator::generateUsers(1000);
    
    auto end_time = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end_time - start_time);
    
    print_both("Generated " + std::to_string(users.size()) + " users in " + 
               std::to_string(duration.count()) + "ms\n");
    UserGenerator::displayUserStats(users);
    
    // Rodome tik 5 users, nors aišku galime ir daugiau (bet ilgiau laiko užtruks)
    print_both("First 5 users:\n");
    for (int i = 0; i < 5 && i < users.size(); i++) {
        users[i].displayUser();
    }

    // Step 2: Generate around 10000 transactions
    print_both("\nSTEP 2: Transaction generation (around 10000 transactions)\n");
    print_both("----------------------------------------\n");
    std::vector<Transaction> all_transactions = TransactionGenerator::generateTransactions(users, 10000);
    TransactionGenerator::displayTransactionStats(all_transactions);
    
    // Display first 10 transactions as example
    print_both("First 10 transactions:\n");
    for (int i = 0; i < 10 && i < all_transactions.size(); i++) {
        all_transactions[i].displayTransaction();
    }

    // Steps 3-5: Complete blockchain process
    print_both("\nSTEPS 3-5: Blockchain process\n");
    print_both("----------------------------------------\n");
    demonstrateBlockchainProcess(users, all_transactions);
    
    print_both("Viskas!\n");
    
    // Uždarome output file
    output_file.close();
    cout << "\nResults saved to rezultatai.txt" << endl;
    
    return 0;

}

// Demonstruokime blokų grandinių procesą
void demonstrateBlockchainProcess(std::vector<User>& users, 
                                 const std::vector<Transaction>& all_transactions) {
    
    // Create blockchain with difficulty 5 (nors čia gana sunkiai)
    const uint32_t DIFFICULTY = 5;
    const int MAX_TXS_PER_BLOCK = 100;
    const double MINING_REWARD = 50.0;
    
    Blockchain blockchain(DIFFICULTY, MINING_REWARD);
    
    print_both("\nBlockchain Configuration:\n");
    print_both("- Difficulty Target: " + std::to_string(DIFFICULTY) + " leading zeros\n");
    print_both("- Max Transactions per Block: " + std::to_string(MAX_TXS_PER_BLOCK) + "\n");
    print_both("- Mining Reward: " + std::to_string(MINING_REWARD) + "\n");
    print_both("- Total Transactions to Process: " + std::to_string(all_transactions.size()) + "\n\n");
    
    // Process transactions in batches until all transactions are processed
    int block_num = 0;
    while (true) {
        print_both("\n--- MINING BLOCK #" + std::to_string(block_num + 1) + " ---\n");
        // Step 3: Block Formation - add next batch of transactions to the pending pool
        int start_idx = block_num * MAX_TXS_PER_BLOCK;
        if (start_idx >= all_transactions.size() && blockchain.getPendingTransactions().empty()) break;

        int end_idx = std::min(start_idx + MAX_TXS_PER_BLOCK, (int)all_transactions.size());
        print_both("Step 3 - Block Formation:\n");
        print_both("  Selected " + std::to_string(end_idx - start_idx) + " transactions from pool\n");
        for (int i = start_idx; i < end_idx; i++) {
            blockchain.addTransaction(all_transactions[i]);
        }
        
        // Step 4: Mining (Proof-of-Work)
        print_both("\nStep 4 - Mining (Proof-of-Work):\n");
        string miner_address = "Miner" + std::to_string(block_num + 1);
        
        auto mine_start = high_resolution_clock::now();
        blockchain.minePendingTransactions(miner_address, const_cast<std::vector<User>&>(users), MAX_TXS_PER_BLOCK);
        auto mine_end = high_resolution_clock::now();
        auto mine_duration = duration_cast<milliseconds>(mine_end - mine_start);
        
        print_both("  Mining completed in " + std::to_string(mine_duration.count()) + "ms\n");
        
        // Step 5: Block Validation
        print_both("\nStep 5 - Validation:\n");
        if (blockchain.isChainValid()) {
            print_both("  Block validated and added to chain\n");
            print_both("  Miner " + miner_address + " received reward: " + 
                      std::to_string(MINING_REWARD) + "\n");
        } else {
            print_both(" Validation FAILED\n");
        }
        block_num++;
    }
    
    // Final blockchain summary
    print_both("\n\n--- Blockchain Summary ---\n");
    print_both("Total Blocks: " + std::to_string(blockchain.getChainSize()) + "\n");
    print_both("Total Transactions Processed: " + 
               std::to_string((blockchain.getChainSize() - 1) * MAX_TXS_PER_BLOCK) + "\n");
    print_both("Chain Valid: " + string(blockchain.isChainValid() ? "YES" : "NO") + "\n");
    
    print_both("\nMiner Balances:\n");
    for (int i = 1; i <= block_num; i++) {
        string miner = "Miner" + std::to_string(i);
        double balance = blockchain.getBalance(miner);
        if (balance > 0) {
            print_both("  " + miner + ": " + std::to_string(balance) + "\n");
        }
    }
    
    // Display detailed blockchain structure
    blockchain.displayChain();
}