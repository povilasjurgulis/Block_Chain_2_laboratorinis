#include "Block.h"
#include <ctime>
#include <random>

Block::Block() : nonce(0), difficulty_target(4), is_mined(false) {
    timestamp = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    version = "v0.2";
}

Block::Block(const string& prev_hash, const std::vector<Transaction>& txs, uint32_t difficulty) 
    : previous_block_hash(prev_hash), transactions(txs), difficulty_target(difficulty), 
      nonce(0), is_mined(false) {
    timestamp = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    version = "v0.2";
    computeMerkleRoot();
}

string Block::getPreviousBlockHash() const {
    return previous_block_hash;
}

uint64_t Block::getTimestamp() const {
    return timestamp;
}

string Block::getVersion() const {
    return version;
}

string Block::getMerkleRootHash() const {
    return merkle_root_hash;
}

uint64_t Block::getNonce() const {
    return nonce;
}

uint32_t Block::getDifficultyTarget() const {
    return difficulty_target;
}

std::vector<Transaction> Block::getTransactions() const {
    return transactions;
}

string Block::getBlockHash() const {
    return block_hash;
}

bool Block::getIsMined() const {
    return is_mined;
}

void Block::mineBlock() {
    string target(difficulty_target, '0');
    
    auto start_time = high_resolution_clock::now();
    
    // Print header preview (short hashes, ISO timestamp, tx root)
    print_both("Header Preview:\n");
    print_both("  Prev Hash: " + previous_block_hash.substr(0, 12) + "...\n");
    // Format timestamp (milliseconds -> ISO 8601 UTC)
    time_t secs = timestamp / 1000;
    struct tm tm_utc;
    gmtime_s(&tm_utc, &secs);
    char buf[32];
    strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", &tm_utc);
    print_both(string("  Timestamp: ") + buf + "\n");
    print_both("  TxRoot: " + merkle_root_hash.substr(0, 12) + "...\n");
    print_both("  Difficulty Target: " + target + "\n");
    
    while (true) {
        block_hash = calculateBlockHash();
        
        if (block_hash.substr(0, difficulty_target) == target) {
            auto end_time = high_resolution_clock::now();
            auto duration = duration_cast<milliseconds>(end_time - start_time);
            
            print_both("  Target: " + target + "...\n");
            print_both("  Found Nonce: " + std::to_string(nonce) + "\n");
            print_both("  Block Hash: " + block_hash.substr(0, 20) + "...\n");
            
            is_mined = true;
            break;
        }
        
        nonce++;
    }
}

bool Block::tryMineForDuration(uint64_t timeLimitMs, uint64_t maxAttempts, uint64_t &attemptsDone, uint64_t &elapsedMs, std::atomic<bool>* stopFlag) {
    using namespace std::chrono;
    string target(difficulty_target, '0');
    auto start_time = steady_clock::now();
    attemptsDone = 0;
    const uint64_t checkInterval = 1024;

    while (attemptsDone < maxAttempts) {
        // If another thread signaled to stop, abort
        if (stopFlag && stopFlag->load()) break;

        block_hash = calculateBlockHash();
        if (block_hash.substr(0, difficulty_target) == target) {
            is_mined = true;
            // signal others if requested
            if (stopFlag) stopFlag->store(true);
            auto end_time = steady_clock::now();
            elapsedMs = duration_cast<milliseconds>(end_time - start_time).count();
            return true;
        }

        nonce++;
        attemptsDone++;

        if ((attemptsDone & (checkInterval - 1)) == 0) {
            // allow external stop or time check
            if (stopFlag && stopFlag->load()) break;
            auto now = steady_clock::now();
            if (duration_cast<milliseconds>(now - start_time).count() >= (int64_t)timeLimitMs) break;
        }
    }

    auto end_time = steady_clock::now();
    elapsedMs = duration_cast<milliseconds>(end_time - start_time).count();
    return false;
}

void Block::setNonce(uint64_t n) {
    nonce = n;
    // reset mined state/hash
    is_mined = false;
    block_hash.clear();
}

string Block::calculateBlockHash() const {
    string header_data = getHeaderString();
    return hash_function(header_data);
}

bool Block::isValidBlock() const {
    // Verify merkle root
    string computed_merkle = MerkleTree::computeMerkleRoot(transactions);
    if (computed_merkle != merkle_root_hash) {
        return false;
    }
    
    // Verify each transaction's ID (recompute and compare)
    for (const auto& tx : transactions) {
        std::string txdata = tx.toString();
        std::string recomputed = hash_function(txdata);
        if (recomputed != tx.getTransactionId()) {
            return false;
        }
    }

    // Verify block hash
    string computed_hash = calculateBlockHash();
    if (computed_hash != block_hash) {
        return false;
    }
    
    // Verify difficulty target
    string target(difficulty_target, '0');
    if (block_hash.substr(0, difficulty_target) != target) {
        return false;
    }
    
    return true;
}

void Block::displayBlock() const {
    print_both("  Previous Hash: " + previous_block_hash.substr(0, 16) + "...\n");
    print_both("  Block Hash: " + block_hash.substr(0, 16) + "...\n");
    print_both("  Merkle Root: " + merkle_root_hash.substr(0, 16) + "...\n");
    print_both("  Nonce: " + std::to_string(nonce) + "\n");
    // Format timestamp to human readable ISO
    time_t secs = timestamp / 1000;
    struct tm tm_utc;
    gmtime_s(&tm_utc, &secs);
    char buf[32];
    strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", &tm_utc);
    print_both(string("  Timestamp: ") + buf + "\n");
    print_both("  Transactions: " + std::to_string(transactions.size()) + "\n");
    
    print_both("\n--- Transactions ---\n");
    for (size_t i = 0; i < transactions.size(); i++) {
        print_both("TX" + std::to_string(i + 1) + ": " + transactions[i].getFromAddress().substr(0, 8) + "... -> " +
                  transactions[i].getToAddress().substr(0, 8) + "... (" + 
                  std::to_string(transactions[i].getAmount()) + ")\n");
    }
}

void Block::displayHeader() const {
    // Hash (short)
    print_both("  Hash: " + block_hash.substr(0, 16) + "...\n");

    // Prev (short) - show '0...' for genesis previous hash
    if (previous_block_hash == "0") {
        print_both("  Prev: 0...\n");
    } else {
        print_both("  Prev: " + previous_block_hash.substr(0, 16) + "...\n");
    }

    // Tx Count
    print_both("  Tx Count: " + std::to_string(transactions.size()) + "\n");

    // Nonce
    print_both("  Nonce: " + std::to_string(nonce) + "\n");

    // Timestamp formatted as ISO 8601 UTC
    time_t secs = timestamp / 1000;
    struct tm tm_utc;
    gmtime_s(&tm_utc, &secs);
    char buf[32];
    strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", &tm_utc);
    print_both(string("  Timestamp: ") + buf + "\n");
}

void Block::computeMerkleRoot() {
    merkle_root_hash = MerkleTree::computeMerkleRoot(transactions);
}

string Block::getHeaderString() const {
    return previous_block_hash + std::to_string(timestamp) + version + 
           merkle_root_hash + std::to_string(nonce) + std::to_string(difficulty_target);
}
