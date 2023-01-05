//
//  NeedlemanWunsch.h
//  src
//
//  Created by Rachel Brockman on 10/16/21.
//

#pragma once
#include "ReadFasta.h"
enum Direction : char
{
    ABOVE_LEFT,
    LEFT,
    ABOVE
};

class NeedlemanWunsch
{
public:
    NeedlemanWunsch(const std::string& fileName1, const std::string& fileName2);
    void RunAlgorithm();
    void Output(const std::string& fileName);
private:
    ReadFasta mFile1;
    ReadFasta mFile2;
    short int mGapScore = -1;
    short int mMatchScore = 1;
    short int mMismatchScore = -1;
    short int mFinalScore = 0;
    std::string mSequenceA;
    std::string mSequenceB;
};
