#ifndef MAIN_H
#define MAIN_H

#include <iostream>
#include <unordered_map>
#include <chrono>
#include <random>
#include <string>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <set>
#include <map>
#include <iomanip>
#include <vector>
#include <algorithm>

using std::unordered_map; using std::cout; using std::ifstream; using std::endl;
using namespace std::chrono; using std::random_device; using std::mt19937; using std::cin;
using std::string;

string hash_function(string &key);

#endif