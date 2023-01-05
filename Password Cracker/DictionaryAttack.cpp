//
//  DictionaryAttack.cpp
//  src
//
//  Created by Rachel Brockman on 9/26/21.
//

#include "DictionaryAttack.h"
#include "sha1.h"
#include "HashTable.hpp"
#include <fstream>
#include <iostream>

HashTable<std::string> ReadDictionaryFile(const std::string& fileName, int tableSize){
    std::ifstream fileInput(fileName);
    HashTable<std::string> dictionary(tableSize);
    
    if(fileInput.is_open()){
        while(!fileInput.eof()){
            std::string line;
            std::getline(fileInput, line);
            unsigned char hash[20];
            sha1::Calc(line.c_str(), line.length(), hash);
            std::string hexStr;
            hexStr.resize(40);
            sha1::ToHexString(hash, &hexStr[0]);
            dictionary.Insert(hexStr, line);
        }
    }
    fileInput.close();
    return dictionary;
}

void Solve(const std::string& fileName, HashTable<std::string>& dictionary){
    std::ifstream fileInput(fileName);
    std::vector<std::string> passwords;
    std::vector<std::pair<std::string, std::string>> solvedPasswords;
    if(fileInput.is_open()){
        passwords.clear();
        while(!fileInput.eof()){
            std::string line;
            std::getline(fileInput, line);
            passwords.emplace_back(line);
        }
    }
    fileInput.close();
    for(size_t i = 0; i < passwords.size(); i++){
        std::string* valuePtr = dictionary.Find(passwords[i]);
        std::pair<std::string, std::string> temp  = std::make_pair(passwords[i], valuePtr->data());
        solvedPasswords.emplace_back(temp);
    }
    std::ofstream outputFile("solved.txt");
    if (outputFile.is_open()){
        for(size_t i = 0; i<solvedPasswords.size(); i++){
            outputFile << solvedPasswords[i].first << "," << solvedPasswords[i].second << std::endl;
        }
    }
    outputFile.close();
}
