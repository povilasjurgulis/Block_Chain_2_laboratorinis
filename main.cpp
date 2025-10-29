#include <iostream>
#include "main.h"
#include "all_classes.h"
#include "Blockchain.h"
#include <chrono>
#include <fstream>

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

void demonstrateBlockchainProcess(std::vector<User>& users, 
                                 const std::vector<Transaction>& all_transactions);

int main()
{
    std::cout << "Hello!";
}