//
//  DictionaryAttack.h
//  src
//
//  Created by Rachel Brockman on 9/26/21.
//

#include <vector>
#include "HashTable.hpp"
#include <utility>
#pragma once

HashTable<std::string> ReadDictionaryFile(const std::string& fileName, int tableSize);
void Solve(const std::string& fileName, HashTable<std::string>& dictionary);
