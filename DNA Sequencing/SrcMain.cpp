#include "SrcMain.h"
#include "ReadFasta.h"
#include "AAStateMachine.h"
#include "NeedlemanWunsch.h"
#include <iostream>
#include <vector>

void ProcessCommandArgs(int argc, const char* argv[])
{
    if(argc == 2){
        std::string fileName = argv[1];
        ReadFasta fastaFile = ReadFasta(fileName);
        AAStateMachine machine = AAStateMachine();
        machine.ProduceAminoAcids(fastaFile.GetSequence());
        machine.OutputAminoAcids(fastaFile, "amino.txt");
//        std::cout << "" << std::endl;
    }
    else if(argc == 3){
        std::string file1 = argv[1];
        std::string file2 = argv[2];
        NeedlemanWunsch test = NeedlemanWunsch(file1, file2);
        test.RunAlgorithm();
        test.Output("match.txt");
    }
}
