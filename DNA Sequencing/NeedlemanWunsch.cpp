//
//  NeedlemanWunsch.cpp
//  src
//
//  Created by Rachel Brockman on 10/16/21.
//

#include "NeedlemanWunsch.h"
#include <vector>
#include <string>
#include <fstream>

NeedlemanWunsch::NeedlemanWunsch(const std::string& fileName1, const std::string& fileName2)
:mFile1(ReadFasta(fileName1))
,mFile2(ReadFasta(fileName2))
{
}

void NeedlemanWunsch::RunAlgorithm()
{
    size_t cols = mFile1.GetSize()+1;
    size_t rows = mFile2.GetSize()+1;
    std::vector<std::vector<short>> scores(rows, std::vector<short>(cols));
    std::vector<std::vector<Direction>> directions(rows, std::vector<Direction>(cols));
    
    scores[0][0] = 0;
    
    for(short int i = 1; i < cols; i++){
        scores[0][i] = -i;
        directions[0][i] = LEFT;
    }
    
    for(short int i = 1; i < rows; i++){
        scores[i][0] = -i;
        directions[i][0] = ABOVE;
    }
    
    short int aboveLeftAndMatch = 0;
    short int leftAndGap = 0;
    short int aboveAndGap = 0;
    char a = 0;
    char b = 0;
    
    for(size_t i = 1; i < rows; i++){
        for(size_t j = 1; j < cols; j++){
            a = mFile1.GetCharacterAtIndex(j-1);
            b = mFile2.GetCharacterAtIndex(i-1);
            
            if(a == b){
                aboveLeftAndMatch = scores[i-1][j-1] + mMatchScore;
            }
            else{
                aboveLeftAndMatch = scores[i-1][j-1] + mMismatchScore;
            }
            
            leftAndGap = scores[i][j-1]+mGapScore;
            aboveAndGap = scores[i-1][j]+mGapScore;
            
            if(aboveLeftAndMatch >= leftAndGap && aboveLeftAndMatch >= aboveAndGap){
                scores[i][j] = aboveLeftAndMatch;
                directions[i][j] = ABOVE_LEFT;
            }
            else if(leftAndGap >= aboveAndGap){
                scores[i][j] = leftAndGap;
                directions[i][j] = LEFT;
            }
            else{
                scores[i][j] = aboveAndGap;
                directions[i][j] = ABOVE;
            }
        }
    }
    
    std::string sequenceA;
    std::string sequenceB;
    size_t row = rows - 1;
    size_t col = cols - 1;

    mFinalScore = scores[row][col];
    
    while(row != 0 || col != 0){
        switch(directions[row][col]){
            case(ABOVE_LEFT):
                sequenceA += mFile1.GetCharacterAtIndex(col-1);
                sequenceB += mFile2.GetCharacterAtIndex(row-1);
                --row;
                --col;
                break;
            case(LEFT):
                sequenceA += mFile1.GetCharacterAtIndex(col-1);
                sequenceB += '_';
                --col;
                break;
            case(ABOVE):
                sequenceA += '_';
                sequenceB += mFile2.GetCharacterAtIndex(row-1);
                --row;
                break;
        }
    }
    std::reverse(sequenceA.begin(), sequenceA.end());
    std::reverse(sequenceB.begin(), sequenceB.end());
    mSequenceA = sequenceA;
    mSequenceB = sequenceB;
}

void NeedlemanWunsch::Output(const std::string& fileName){
    std::ofstream outputFile(fileName);
    if(outputFile.is_open()){
        outputFile << "A: " << mFile1.GetHeader() << std::endl;
        outputFile << "B: " << mFile2.GetHeader() << std::endl;
        outputFile << "Score: " << mFinalScore << "\n" << std::endl;
        
        int numLines = mSequenceA.length()/70;
        int remainder = mSequenceA.length() % 70;
        
        for(size_t i = 0; i < numLines; i++){
            for(size_t j = 0 + (i*70); j < 70 + (i*70); j++){
                outputFile << mSequenceA[j];
            }
            outputFile << std::endl;
            for(size_t j = 0 + (i*70); j < 70 + (i*70); j++){
                if(mSequenceA[j] == mSequenceB[j]){
                    outputFile << "|";
                }
                else{
                    outputFile << " ";
                }
            }
            outputFile << std::endl;
            for(size_t j = 0 + (i*70); j < 70 + (i*70); j++){
                outputFile << mSequenceB[j];
            }
            outputFile << std::endl;
            outputFile << std::endl;
        }
        if(remainder != 0){
            int remainderIndex = 70*numLines;
            for(size_t i = remainderIndex; i<mSequenceA.length(); i++){
                outputFile << mSequenceA[i];
            }
            outputFile << std::endl;
            for(size_t i = remainderIndex; i<mSequenceA.length(); i++){
                if(mSequenceA[i] == mSequenceB[i]){
                    outputFile << "|";
                }
                else{
                    outputFile << " ";
                }
            }
            outputFile << std::endl;
            for(size_t i = remainderIndex; i<mSequenceA.length(); i++){
                outputFile << mSequenceB[i];
            }
            outputFile << std::endl;
            outputFile << std::endl;
        }
        
    }
}
