#include "RleData.h"

void RleData::Compress(const std::vector<int8_t>& input)
{
    mData.clear();
    mData.reserve(input.size()*2);
    std::vector<int8_t> repeats;
    std::vector<int8_t> pass1;
    int numRepeats = 1;
    repeats.push_back(input.front());
    if(input.size() == 1){
        pass1.push_back(1);
        pass1.push_back(input[0]);
    }
    for (int i=1; i<input.size(); i++){
        //        Pass 1: Assume only positive runs and store the result in a temp vector
        if(input[i] == input[i-1]){
            repeats.push_back(input[i]);
            numRepeats += 1;
            //If the run reaches a length of 127, add it to the pass1 vector and reset the counter
            if(numRepeats == 127){
                pass1.push_back(numRepeats);
                pass1.push_back(repeats[0]);
                numRepeats = 0;
                repeats.clear();
            }
        }
        
        else{
            pass1.push_back(numRepeats);
            pass1.push_back(repeats[0]);
            repeats.clear();
            repeats.push_back(input[i]);
            numRepeats = 1;
        }
        if (i == input.size()-1){
            pass1.push_back(numRepeats);
            pass1.push_back(repeats[0]);
        }
    }
    //        Pass 2: Find any neighboring runs of size 1 and change them into a negative run
    std::vector<int8_t> negativeRun;
    std::vector<int> negRunIndices;
    int negRunCounter = 0;
    int numOfLoop = 1;
    for(int i=0; i < pass1.size(); i++){
        //Reviewing the even indicies
        if (i % 2 == 0){
            //The counter value is not 1, so we just need to write the counter value and the corresponding char directly from the temp vector
            if(pass1[i] != 1){
                mData.push_back(pass1[i]);
                mData.push_back(pass1[i+1]);
            }
            //The counter value is 1, so we need to check to see if this is actually a negative run or not
            if(pass1[i] == 1){
                //This means that there are at least 2 more values to be added to the negative run -- not at the end yet
//                Need to add additional boundary checks (not guaranteed that these are valid indicies)
                if((pass1[i+2] == 1) && (pass1[i+4] == 1)){
                    negativeRun.push_back(pass1[i+1]);
                    negRunCounter -= 1;
                }
                //reached the end of the negative run, but not yet at the end of end of the temp vector
                else if((pass1[i+2] == 1) && (i != pass1.size()-1)){
                    negativeRun.push_back(pass1[i+1]);
                    negativeRun.push_back(pass1[i+3]);
                    negRunCounter -= 2;
                    if(negativeRun.size() > 127){
                        int counter = - negativeRun.size();
                        int loopNumber = 0;
                        while(counter < 0){
                            if(counter + 127 < 0){
                                mData.push_back(-127);
                                for(int j = 127*loopNumber; j < 127 + (127*loopNumber); j++){
                                    mData.push_back(negativeRun[j]);
                                    counter++;
                                }
                            }
                            else{
                                if(counter == -1){
                                    mData.push_back(1);
                                }
                                else{
                                    mData.push_back(counter);
                                }
                                for(int k = 127*loopNumber; k < negativeRun.size(); k++){
                                    mData.push_back(negativeRun[k]);
                                    counter++;
                                }
                            }
                            loopNumber++;
                        }
                    }
                    else{
                        mData.push_back(negRunCounter);
                        //Does this affect performance?
                        for (int j = 0; j < negativeRun.size(); j++){
                            mData.push_back(negativeRun[j]);
                        }
                    }
                    negRunCounter = 0;
                    negativeRun.clear();
                }
                
                //end of the negative run and also end of temp
                else if(pass1[i+2] == 1){
                    //Not 100% sure about this logic
                    negativeRun.push_back(pass1[i+1]);
                    negRunCounter -= 1;
                }
                //Just a singular value, not a negative run
                else if (i < 2 || pass1[i-2] != 1){
                    mData.push_back(pass1[i]);
                    mData.push_back(pass1[i+1]);
                }
            }
        }
    }
}

void RleData::Decompress(const std::vector<int8_t>& input, size_t outSize)
{
    mData.clear();
    mData.reserve(outSize);
    
    
    int negRunCounter = 1;
    int nextIndex = 0;
    for(int i = 0; i<input.size();i++){
        if (i == nextIndex){
            if(negRunCounter > 0){
                if(input[i]>0){
                    for(int j = 0; j < input[i]; j++){
                        mData.push_back(input[i+1]);
                    }
                    nextIndex += 2;
                }
                else{
                    negRunCounter =  input[i];
                    for(int k = i+1; k < -input[i]+i+1; k++){
                        mData.push_back(input[k]);
                    }
                    nextIndex += (-input[i])+1;
                }
                
            }
        }
        
        negRunCounter++;
    }
    
}

std::ostream& operator<< (std::ostream& stream, const RleData& rhs)
{
    for (size_t i = 0; i < rhs.mData.size(); i++)
    {
        stream << rhs.mData[i];
    }
    return stream;
}
