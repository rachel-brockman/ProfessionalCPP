//
//  ReadFasta.h
//  src
//
//  Created by Rachel Brockman on 10/14/21.
//

#pragma once
#include <string>
#include <iostream>
class ReadFasta
{
public:
    ReadFasta(const std::string& fileName);
    void ReadFile(const std::string& fileName);
    std::string GetSequence();
    std::string GetHeader();
    size_t GetSize();
    char GetCharacterAtIndex(short int i);
private:
    std::string mHeader;
    std::string mSequence;
};
