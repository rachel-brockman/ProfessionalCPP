#include "SrcMain.h"
#include "DictionaryAttack.h"
#include "BruteForce.h"
#include <iostream>
#include <cstring>

void ProcessCommandArgs(int argc, const char* argv[])
{
	// TODO: Implement
    if(strcmp(argv[1], "dictionary") == 0){
        int size = std::stoi(argv[2]);
        std::string dictionaryFile = argv[3];
        std::string passwordFile = argv[4];
        HashTable<std::string> test = ReadDictionaryFile(dictionaryFile, size);
        Solve(passwordFile, test);
    }
    else if (strcmp(argv[1], "phrases") == 0){
        int size = std::stoi(argv[2]);
        std::string words = argv[3];
        std::string passwordFile = argv[4];
        std::vector<std::string> fileWords = ReadWordsFromFile(words, size);
        CreateDictionary(fileWords, size, passwordFile);
    }
}
