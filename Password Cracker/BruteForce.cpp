//
//  BruteForce.cpp
//  src
//
//  Created by Rachel Brockman on 9/26/21.
//

#include "BruteForce.h"
#include "sha1.h"
#include <algorithm>
#include <iostream>
#include <vector>
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/parallel_for_each.h>

std::vector<std::string> ReadWordsFromFile(const std::string& fileName, int size){
    std::ifstream fileInput(fileName);
    std::vector<std::string> words;
    
    if(fileInput.is_open()){
        words.clear();
        std::string line;
        while(!fileInput.eof()){
            std::getline(fileInput, line);
            words.emplace_back(line);
        }
    }
    fileInput.close();
    return words;
}

void CreateDictionary(std::vector<std::string>& permutations, int tableSize, std::string& fileName){
    HashTable<std::string> dictionary(tableSize);
    std::ifstream fileInput(fileName);
    HashTable<int> lineNumbers(tableSize);
    int counter = 0;
    if(fileInput.is_open()){
        std::string line;
        while(!fileInput.eof()){
            std::getline(fileInput, line);
            lineNumbers.Insert(line, counter);
            dictionary.Insert(line, "");
            counter++;
        }
    }
    fileInput.close();
    std::vector<std::pair<std::string, std::string>> solvedPasswords(counter);
    tbb::parallel_for(tbb::blocked_range<size_t>(0, permutations.size()), [&dictionary, &solvedPasswords, &lineNumbers, &permutations](const tbb::blocked_range<size_t>& r){
        std::string word1;
        std::string word2;
        std::string word3;
        std::string word4;
        std::string outputWord;
        unsigned char hash[20];
        for(size_t i = r.begin(); i != r.end(); i++){
            word1 = permutations[i];
            for(size_t j = 0; j < permutations.size(); j++){
                word2 = permutations[j];
                for(size_t k = 0; k < permutations.size(); k++){
                    word3 = permutations[k];
                    for(size_t l = 0; l < permutations.size(); l++){
                        word4 = permutations[l];
                        outputWord.clear();
                        outputWord += word1;
                        outputWord += word2;
                        outputWord += word3;
                        outputWord += word4;
                        sha1::Calc(outputWord.c_str(), outputWord.length(), hash);
                        std::string hexStr;
                        hexStr.resize(40);
                        sha1::ToHexString(hash, &hexStr[0]);
                        std::string* valuePtr = dictionary.Find(hexStr);
                        int* lineNumPtr = lineNumbers.Find(hexStr);
                        if(valuePtr != nullptr){
                            *valuePtr = permutations[i];
                            std::pair<std::string, std::string> pair = std::make_pair(hexStr, outputWord);
                            if(lineNumPtr != nullptr){
                                solvedPasswords[*lineNumPtr] = pair;
                            }
                        }

                    }
                }
            }
        }
    });
    std::ofstream outputFile("solved.txt");
    if (outputFile.is_open()){
        for(size_t i = 0; i<solvedPasswords.size(); i++){
            outputFile << solvedPasswords[i].first << "," << solvedPasswords[i].second << std::endl;
        }
    }
    outputFile.close();
}
