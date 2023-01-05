//
//  BruteForce.h
//  src
//
//  Created by Rachel Brockman on 9/26/21.
//

#pragma once
#include <fstream>
#include <vector>
#include "HashTable.hpp"
#include <utility>

std::vector<std::string> ReadWordsFromFile(const std::string& fileName, int size);
void CreateDictionary(std::vector<std::string>& permutations, int tableSize, std::string& fileName);

