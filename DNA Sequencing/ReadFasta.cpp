//
//  ReadFasta.cpp
//  src
//
//  Created by Rachel Brockman on 10/14/21.
//

#include "ReadFasta.h"
#include <fstream>
#include <vector>
ReadFasta::ReadFasta(const std::string& fileName)
{
    ReadFile(fileName);
}

void ReadFasta::ReadFile(const std::string &fileName)
{
    std::ifstream::pos_type size;
    // Open the file for input and start ATE (at the end)
    std::ifstream fileInput(fileName, std::ios::in|std::ios::ate);
    if(fileInput.is_open())
    {
        size = fileInput.tellg(); // Save the size of the file
        mSequence.reserve(size);
        fileInput.seekg(0, std::ios::beg); // Seek back to start of file
        std::getline(fileInput, mHeader);
        mHeader = mHeader.substr(1, mHeader.size());
        std::string line;
        while(!fileInput.eof()){
            std::getline(fileInput, line);
            for(size_t i = 0; i<line.size(); i++){
                if(line[i] == 'A' || line[i] == 'T' || line[i]== 'C' || line[i] == 'G'){
                    mSequence += line[i];
                }
            }
        }
        fileInput.close();
    }
}

std::string ReadFasta::GetSequence(){
    return mSequence;
}

std::string ReadFasta::GetHeader(){
    return mHeader;
}

size_t ReadFasta::GetSize(){
    return mSequence.size();
}

char ReadFasta::GetCharacterAtIndex(short int i){
    return mSequence[i];
}
