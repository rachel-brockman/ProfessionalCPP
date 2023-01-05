//
//  AAStateMachine.cpp
//  src
//
//  Created by Rachel Brockman on 10/14/21.
//

#include "AAStateMachine.h"
#include "ReadFasta.h"
#include <iostream>
#include <fstream>
#include <vector>

AAStateMachine::AAStateMachine() = default;

void AAStateMachine::ProduceAminoAcids(const std::string &dnaSequence)
{
    int state = 0;
    std::string test;
    for(char letter : dnaSequence){
        int row = state;
        int col = 0;
        switch (letter) {
            case 'T':
                col = 0;
                break;
            case 'C':
                col = 1;
                break;
            case 'A':
                col = 2;
                break;
            case 'G':
                col = 3;
                break;
            default:
                break;
        }
        state = mStateTable[row][col];
        if(state >= 'A'){
            auto itr = mAminoAcidCount.find(state);
            itr->second += 1;
            mTotalAminoAcids++;
            state = 3;
            
        }
    }
}

void AAStateMachine::OutputAminoAcids(ReadFasta& fastaFile, const std::string& outputFile)
{
    std::ofstream output(outputFile);
    if(output.is_open()){
        output << fastaFile.GetHeader() << std::endl;
        output<< "Total amino acids produced: " << mTotalAminoAcids << std::endl;
        for(std::pair pair: mAminoAcidCount){

            auto itr = mAminoAcidAbbreviations.find(pair.first);
            output << "(" << pair.first << ") " << itr->second << ": " << pair.second << std::endl;

        }
    }
    output.close();
}
