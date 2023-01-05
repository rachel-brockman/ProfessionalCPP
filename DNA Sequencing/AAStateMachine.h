//
//  AAStateMachine.h
//  src
//
//  Created by Rachel Brockman on 10/14/21.
//

#pragma once
#include <string>
//#include <utility>
//#include <vector>
#include "ReadFasta.h"
#include <map>

class AAStateMachine
{
public:
    AAStateMachine();
    void ProduceAminoAcids(const std::string& dnaSequence);
    void OutputAminoAcids(ReadFasta& fastaFile, const std::string& outputFile);
private:
    int mStateTable[24][4] = {
        {0, 0, 1, 0},
        {2, 0, 1, 0},
        {0, 0, 1, 'M'},
        {4, 9, 14, 19},
        {5, 6, 7, 8},
        {'F', 'F', 'L', 'L'},
        {'S', 'S', 'S', 'S'},
        {'Y', 'Y', 0, 0},
        {'C', 'C', 0, 'W'},
        {10, 11, 12, 13},
        {'L', 'L', 'L', 'L'},
        {'P', 'P', 'P', 'P'},
        {'H', 'H', 'Q', 'Q'},
        {'R', 'R', 'R', 'R'},
        {15, 16, 17, 18},
        {'I', 'I', 'I', 'M'},
        {'T', 'T', 'T', 'T'},
        {'N', 'N', 'K', 'K'},
        {'S', 'S', 'R', 'R'},
        {20, 21, 22, 23},
        {'V', 'V', 'V', 'V'},
        {'A', 'A', 'A', 'A'},
        {'D', 'D', 'E', 'E'},
        {'G', 'G', 'G', 'G'}
    };
    std::map<char, int> mAminoAcidCount = {
        {'A', 0},
        {'C', 0},
        {'D', 0},
        {'E', 0},
        {'F', 0},
        {'G', 0},
        {'H', 0},
        {'I', 0},
        {'K', 0},
        {'L', 0},
        {'M', 0},
        {'N', 0},
        {'P', 0},
        {'Q', 0},
        {'R', 0},
        {'S', 0},
        {'T', 0},
        {'V', 0},
        {'W', 0},
        {'Y', 0}
    };
    std::map<char, std::string> mAminoAcidAbbreviations = {
        {'A', "Alanine"},
        {'C', "Cysteine"},
        {'D', "Aspartic acid"},
        {'E', "Glutamic acid"},
        {'F', "Phenylalanine"},
        {'G', "Glycine"},
        {'H', "Histidine"},
        {'I', "Isoleucine"},
        {'K', "Lysine"},
        {'L', "Leucine"},
        {'M', "Methionine"},
        {'N', "Asparagine"},
        {'P', "Proline"},
        {'Q', "Glutamine"},
        {'R', "Arginine"},
        {'S', "Serine"},
        {'T', "Threonine"},
        {'V', "Valine"},
        {'W', "Tryptophan"},
        {'Y', "Tyrosine"}
    };
    int mTotalAminoAcids = 0;
};
